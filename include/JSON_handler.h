//
// Created by vanho on 03/08/2025.
//

#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#endif //JSON_HANDLER_H

#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "libmp3player.h"

using json = nlohmann::json;
using std::string;

class Song {
private:
    string path;
    string name;
    string author;
    string album;
    string genre;
    int year = 0;
    int track_number = 0;
    int disc_number = 0;

public:
    // Constructors
    Song() = default;
    Song(string p, string n, string a, string al, string g, int y, int tn, int dn)
        : path(p), name(n), author(a), album(al), genre(g),
          year(y), track_number(tn), disc_number(dn) {}

    // Getters
    string Get_Author() const { return author; }
    string Get_Genre() const { return genre; }
    string Get_Album() const { return album; }
    string Get_Name() const { return name; }
    int Get_TrackNumber() const { return track_number; }
    int Get_DiscNumber() const { return disc_number; }
    int Get_Year() const { return year; }

    void Play() {
        playMP3(path);
    }

    // JSON Serialization
    friend void to_json(json& j, const Song& s) {
        j = json{
                {"path", s.path},
                {"name", s.name},
                {"author", s.author},
                {"album", s.album},
                {"genre", s.genre},
                {"year", s.year},
                {"track_number", s.track_number},
                {"disc_number", s.disc_number}
        };
    }

    friend void from_json(const json& j, Song& s) {
        j.at("path").get_to(s.path);
        j.at("name").get_to(s.name);
        j.at("author").get_to(s.author);
        j.at("album").get_to(s.album);
        j.at("genre").get_to(s.genre);
        j.at("year").get_to(s.year);
        j.at("track_number").get_to(s.track_number);
        j.at("disc_number").get_to(s.disc_number);
    }

    // Utility: Check if file exists
    static bool FileExists(const string& filename) {
        return std::filesystem::exists(filename);
    }

    // Save to JSON file
    bool SaveToFile(const string& filename) const {
        if (!FileExists(filename)) {
            std::cout << "File does not exist. Creating new file: " << filename << "\n";
        } else {
            std::cout << "File exists. Overwriting: " << filename << "\n";
        }

        std::ofstream file(filename);
        if (!file.is_open()) return false;
        json j = *this;
        file << j.dump(4);
        return true;
    }

    // Load from JSON file
    static Song LoadFromFile(const string& filename) {
        if (!FileExists(filename)) {
            throw std::runtime_error("Cannot load: file does not exist -> " + filename);
        }
        std::ifstream file(filename);
        json j;
        file >> j;
        return j.get<Song>();
    }
};

#endif // SONG_HPP