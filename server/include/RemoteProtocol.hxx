/**
 * @file RemoteProtocol.hxx
 * @brief Declaration of the RemoteProtocol class for handling remote communication protocol.
 *
 * @details The RemoteProtocol implements the custom protocol for push/pull operations
 * between SVCS clients and servers. It handles object negotiation, transfer, and
 * reference updates through a simple text-based protocol over stdin/stdout.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
#pragma once

#include "../../services/ISubject.hxx"
#include "../../core/include/RepositoryManager.hxx"

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>
#include <iostream>

/**
 * @class RemoteProtocol
 * @brief Handles the remote communication protocol for SVCS.
 *
 * @ingroup Core
 *
 * @details Manages the custom protocol for **push** and **pull** operations, including
 * object negotiation, data transfer, and reference updates between client and server.
 * The protocol is a simple text-based exchange over standard input/output.
 */
class RemoteProtocol {
public:
    /**
     * @brief Constructs a RemoteProtocol with necessary dependencies.
     *
     * @param event_bus The shared pointer to the event bus for the notification system (@ref ISubject).
     * @param repository_manager The shared pointer to the repository manager for repository operations (@ref RepositoryManager).
     */
    RemoteProtocol(std::shared_ptr<ISubject> event_bus,
                   std::shared_ptr<RepositoryManager> repository_manager);

    /**
     * @brief Default destructor.
     */
    ~RemoteProtocol() = default;

    /**
     * @brief Handles the 'receive-pack' protocol for **push** operations.
     *
     * @return bool True if the protocol completed successfully (all objects received and references updated), false otherwise.
     *
     * @details This function processes client push requests. It manages the negotiation of objects,
     * receives the missing object data from the client, and finally attempts to apply the
     * requested reference updates (e.g., updating branches).
     */
    bool handleReceivePack();

    /**
     * @brief Handles the 'upload-pack' protocol for **pull** operations.
     *
     * @return bool True if the protocol completed successfully (objects negotiated and sent), false otherwise.
     *
     * @details This function processes client pull requests. It performs object negotiation by
     * determining which objects the client is missing based on its 'have' list, and then
     * packages and sends the missing object data to the client.
     */
    bool handleUploadPack();

private:
    /// @name Receive-pack (Push) Protocol Methods
    /// @{
    /**
     * @brief Processes the initial negotiation phase for a push operation.
     *
     * @return bool True on successful negotiation, false otherwise.
     */
    bool processPushNegotiation();

    /**
     * @brief Receives and stores objects sent by the client.
     *
     * @param wanted_objects A set of object hashes that the server is missing and the client needs to send.
     * @return bool True if all expected objects were received and written successfully, false otherwise.
     */
    bool receiveObjects(const std::unordered_set<std::string>& wanted_objects);

    /**
     * @brief Updates local references (e.g., branches, tags) based on client requests.
     *
     * @param ref_updates A vector of pairs where each pair is `{ref_name, new_hash}`.
     * @return bool True if all reference updates were applied successfully, false otherwise.
     */
    bool updateReferences(const std::vector<std::pair<std::string, std::string>>& ref_updates);
    /// @}

    /// @name Upload-pack (Pull) Protocol Methods
    /// @{
    /**
     * @brief Processes the initial negotiation phase for a pull operation.
     *
     * @return bool True on successful negotiation, false otherwise.
     */
    bool processPullNegotiation();

    /**
     * @brief Identifies which objects the client is missing.
     *
     * @param client_has A set of object hashes that the client already has.
     * @return std::unordered_set<std::string> A set of object hashes that the server has, but the client is missing and needs.
     */
    std::unordered_set<std::string> findMissingObjects(const std::unordered_set<std::string>& client_has);

    /**
     * @brief Sends the requested missing objects to the client.
     *
     * @param objects_to_send A set of object hashes to be sent to the client.
     * @return bool True if all specified objects were successfully sent, false otherwise.
     */
    bool sendObjects(const std::unordered_set<std::string>& objects_to_send);
    /// @}

    /// @name Object and Reference Management Methods
    /// @{
    /**
     * @brief Checks if a given object hash exists in the repository.
     *
     * @param object_hash The SHA-1 hash of the object.
     * @return bool True if the object exists, false otherwise.
     */
    bool objectExists(const std::string& object_hash) const;

    /**
     * @brief Reads and retrieves the content of an object.
     *
     * @param object_hash The SHA-1 hash of the object.
     * @return std::string The content of the object, or an empty string on failure.
     */
    std::string readObject(const std::string& object_hash) const;

    /**
     * @brief Writes a new object to the repository.
     *
     * @param object_hash The expected SHA-1 hash of the object (used for verification).
     * @param object_data The raw data of the object to be stored.
     * @return bool True if the object was written successfully, false otherwise.
     */
    bool writeObject(const std::string& object_hash, const std::string& object_data);

    /**
     * @brief Retrieves a set of all current reference names (e.g., branch names, tags) in the repository.
     *
     * @return std::unordered_set<std::string> A set of all reference names.
     */
    std::unordered_set<std::string> getAllReferences() const;

    /**
     * @brief Updates a specific reference to point to a new object hash.
     *
     * @param ref_name The name of the reference (e.g., "refs/heads/master").
     * @param new_hash The new object hash (e.g., commit SHA-1) the reference should point to.
     * @return bool True if the reference was updated, false otherwise.
     */
    bool updateReference(const std::string& ref_name, const std::string& new_hash);
    /// @}

    /// @name Protocol Communication Methods
    /// @{
    /**
     * @brief Reads a single line from the standard input (stdin) for protocol data.
     *
     * @param line Output parameter to store the read line.
     * @return bool True if a line was read successfully, false on EOF or read error.
     *
     * @note This method handles the underlying I/O for the text-based protocol.
     */
    bool readLine(std::string& line) const;

    /**
     * @brief Sends a single line of data to the standard output (stdout) for the protocol.
     *
     * @param line The line of data to send.
     * @return bool True if the line was sent successfully, false otherwise.
     *
     * @note This method handles the underlying I/O for the text-based protocol.
     */
    bool sendLine(const std::string& line) const;

    /**
     * @brief Sends raw data (e.g., packed objects) to the standard output.
     *
     * @param data The raw string data to send.
     * @return bool True if the data was sent successfully, false otherwise.
     */
    bool sendData(const std::string& data) const;

    /**
     * @brief Sends an error message through the protocol channel.
     *
     * @param error The error message string.
     * @return bool True if the error message was sent successfully, false otherwise.
     */
    bool sendError(const std::string& error) const;
    /// @}

    /// @name Validation Methods
    /// @{
    /**
     * @brief Validates if a string is a correctly formatted object hash (e.g., 40 hex characters).
     *
     * @param hash The string to validate.
     * @return bool True if the hash is valid, false otherwise.
     */
    bool isValidObjectHash(const std::string& hash) const;

    /**
     * @brief Validates if a string is a correctly formatted and safe reference name.
     *
     * @param ref_name The string to validate (e.g., "refs/heads/master").
     * @return bool True if the reference name is valid, false otherwise.
     */
    bool isValidReference(const std::string& ref_name) const;
    /// @}

    /// @name Notification Methods
    /// @{
    /**
     * @brief Sends a debug notification message via the event bus.
     *
     * @param message The debug message.
     */
    void notifyDebug(const std::string& message) const;

    /**
     * @brief Sends an informational notification message via the event bus.
     *
     * @param message The informational message.
     */
    void notifyInfo(const std::string& message) const;

    /**
     * @brief Sends an error notification message via the event bus.
     *
     * @param message The error message.
     */
    void notifyError(const std::string& message) const;
    /// @}

    /// @name Private Data Members
    /// @{
    std::shared_ptr<ISubject> event_bus_;           ///< Shared pointer to the event bus for notifications.
    std::shared_ptr<RepositoryManager> repository_manager_; ///< Shared pointer to the repository manager for object and reference operations.
    /// @}
};