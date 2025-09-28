#include "../include/commit.hxx"
#include <iostream>
#include <regex>
#include <utility>

Author::Author() :
    name("Anonym"), email("anonym@gmail.com") {}

Author::Author(std::string n, std::string e) {
    this->name = std::move(n);
    const std::regex pattern(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");

    if (std::regex_match(e, pattern)) {
        this->email = std::move(e);
        std::cerr << "Warning: The provided email '" << e << "' is not valid! Using default email: anonym@gmail.com." << std::endl;
        this->email = "anonym@gmail.com"; 
    }
}

Author::Author(const Author& a) :
    name(a.name), email(a.email) {}

std::string Author::get_name() const { return this->name; }

std::string Author::get_email() const { return this->email; }

// --- Constructors ---

Commit::Commit() 
    : VcsObject(),
      author(), 
      user_message(""), 
      timestamp(""), 
      content_hash(""), 
      file_path("") {}


Commit::Commit(const Commit& c) 
    : VcsObject(c),
      author(c.author), 
      user_message(c.user_message), 
      timestamp(c.timestamp), 
      content_hash(c.content_hash), 
      file_path(c.file_path) {}


Commit::Commit(
    Author auth, std::string umsg, 
    std::string tstamp, std::string chash, std::string fpath
) 
    : VcsObject(),
      author(std::move(auth)),
      user_message(std::move(umsg)), 
      timestamp(std::move(tstamp)), 
      content_hash(std::move(chash)), 
      file_path(std::move(fpath)) 
{
    std::string data_to_hash = this->serialize(); 
    
    this->computeHash(data_to_hash); 
}


std::string Commit::serialize() const {
    return "There should be serealization function!";
}

std::string Commit::getType() const {
    return "Commit";
}


// --- Getters ---

Author Commit::getAuthor() const {return this->author; }

std::string Commit::getUserMessage() const { return this->user_message; }

std::string Commit::getTimestamp() const { return this->timestamp; }

std::string Commit::getContentHash() const { return this->content_hash; }

std::string Commit::getFilePath() const { return this->file_path; }