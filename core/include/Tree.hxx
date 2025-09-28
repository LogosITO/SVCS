/**
 * @file Tree.hxx
 * @brief Defines the Tree class, which represents the hierarchical structure (directory state) of a VCS repository.
 * @details The Tree object stores a list of TreeEntry structures, linking file names and modes to the hash IDs of
 * Blob or other Tree objects. Its hash depends critically on the sorted content of these entries.
 */
#pragma once

#include "VcsObject.hxx"
#include <optional>
#include <string>
#include <vector>

/**
 * @struct TreeEntry
 * @brief Represents a single item (file or subdirectory) within a Tree object.
 * * This structure links a name and file mode to the hash ID of another VCS object.
 */
struct TreeEntry {
    /// @brief File mode (permissions) and type identifier (e.g., "100644" for a blob, "040000" for a tree).
    std::string mode;
    /// @brief The name of the file or subdirectory.
    std::string name;
    /// @brief The hash ID (SHA-256) of the referenced object (Blob or Tree).
    std::string hash_id;
    /// @brief The type of the referenced object ("blob" or "tree").
    std::string type;

    /**
     * @brief Comparison operator required for sorting.
     * @details Entries must be sorted alphabetically by name before serialization to ensure a stable and consistent hash ID for the Tree object.
     * @param other The TreeEntry to compare against.
     * @return bool True if this entry should precede the 'other' entry in the sorted list.
     */
    bool operator<(const TreeEntry& other) const;
};

/**
 * @brief Creating tree entry.
 * @return Returns finished TreeEntry.
 * @param name Name of file or subdirectory.
 * @param hash_id The hash ID (SHA-256).
 */
TreeEntry createEntry(std::string name, std::string hash_id);

/**
 * @class Tree
 * @brief Represents the state of a directory in the VCS, implementing the VcsObject contract.
 * * Stores a collection of TreeEntry objects, effectively mapping names to hash pointers.
 */
class Tree : public VcsObject {
private:
    /// @brief The list of all items contained within this directory.
    std::vector<TreeEntry> entries;
public:
    /**
     * @brief Constructor for the Tree object.
     * @details The constructor must internally sort the entries before calling computeHash().
     * @param entries A collection of TreeEntry structures representing the directory content.
     */
    Tree(std::vector<TreeEntry> entries);

    // VcsObject overrides

    /**
     * @brief Returns the type of the VCS object.
     * @return std::string Always returns "tree".
     * @copydoc VcsObject::getType()
     */
    std::string getType() const override;

    /**
     * @brief Serializes the Tree content into a standardized string format for hashing and storage.
     * @details The serialization must ensure the entries are sorted to guarantee hash consistency.
     * @return std::string The standardized, serialized directory structure.
     * @copydoc VcsObject::serialize()
     */
    std::string serialize() const override;

    /**
     * @brief Creates a Tree object from a serialized string read from the object database.
     * @param raw_content The serialized string data, typically read from a decompressed object file.
     * @return Tree A fully reconstructed Tree object.
     */
    static Tree deserialize(const std::string& raw_content);

    /**
     * @brief Adds a new entry to the Tree or updates an existing entry if the name matches.
     * @details If the entries list is modified, the Tree's hash becomes invalid and must be recomputed before saving.
     * @param entry The TreeEntry to add or update.
     */
    void addEntry(const TreeEntry& entry);

    /**
     * @brief Removes an entry (file or subdirectory) from the Tree by name.
     * @param name The name of the entry to remove.
     * @return bool True if the entry was found and removed, otherwise false.
     */
    bool removeEntry(const std::string& name);

    /**
     * @brief Returns the list of entries stored in the Tree.
     * @return const std::vector<TreeEntry&> A constant reference to the internal vector of entries.
     */
    const std::vector<TreeEntry&> getEntries() const;

    /**
     * @brief Searches for a specific entry within the Tree by name.
     * @param name The name of the entry to find.
     * @return std::optional<TreeEntry> An optional containing the entry if found, otherwise empty.
     */
    std::optional<TreeEntry> findEntry(const std::string& name) const;
    /**
     *@brief Returns the list of entries stored in the Tree.
     *@return std::vector<TreeEntry&> A constant reference to the internal vector of entries.
     */
    std::vector<TreeEntry> getEntries();
};