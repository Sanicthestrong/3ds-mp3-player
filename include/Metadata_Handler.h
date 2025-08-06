//
// Created by vanho on 03/08/2025.
//

#ifndef METADATA_HANDLER_H
#define METADATA_HANDLER_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

struct Mp3Metadata {
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    int year = 0;
    int track = 0;
    int disc = 0;
    std::string albumArtPath;
};

class Mp3MetadataReader {
public:
    static Mp3Metadata ReadMetadata(const std::string& filepath, const std::string& coverOut = "cover.jpg") {
        Mp3Metadata meta;
        std::ifstream file(filepath, std::ios::binary);
        if (!file) throw std::runtime_error("Could not open file: " + filepath);

        // Read header
        char header[10];
        file.read(header, 10);
        if (std::string(header, 3) != "ID3") {
            throw std::runtime_error("No ID3v2 tag found");
        }

        int tagSize = synchsafeToInt(&header[6]);

        int bytesRead = 0;
        while (bytesRead < tagSize) {
            char frameHeader[10];
            file.read(frameHeader, 10);
            bytesRead += 10;

            std::string frameID(frameHeader, 4);
            int frameSize = ((unsigned char)frameHeader[4] << 24) |
                            ((unsigned char)frameHeader[5] << 16) |
                            ((unsigned char)frameHeader[6] << 8)  |
                            ((unsigned char)frameHeader[7]);

            if (frameID.empty() || frameSize <= 0 || frameID[0] == 0) break;

            std::vector<char> frameData(frameSize);
            file.read(frameData.data(), frameSize);
            bytesRead += frameSize;

            if (frameID == "TIT2") meta.title = parseTextFrame(frameData);
            else if (frameID == "TPE1") meta.artist = parseTextFrame(frameData);
            else if (frameID == "TALB") meta.album = parseTextFrame(frameData);
            else if (frameID == "TCON") meta.genre = parseTextFrame(frameData);
            else if (frameID == "TYER") meta.year = std::stoi(parseTextFrame(frameData));
            else if (frameID == "TRCK") meta.track = std::stoi(parseTextFrame(frameData));
            else if (frameID == "TPOS") meta.disc = std::stoi(parseTextFrame(frameData));
            else if (frameID == "APIC") {
                std::string mime;
                size_t i = 1;
                while (i < frameData.size() && frameData[i] != 0) mime += frameData[i++];
                ++i; // skip null
                i += 1; // skip picture type
                while (i < frameData.size() && frameData[i] != 0) ++i; // skip description
                ++i;

                std::ofstream out(coverOut, std::ios::binary);
                out.write(&frameData[i], frameSize - i);
                out.close();
                meta.albumArtPath = coverOut;
            }
        }

        return meta;
    }

private:
    static std::string parseTextFrame(const std::vector<char>& data) {
        if (data.empty()) return "";
        if (data[0] == 0) return std::string(data.begin() + 1, data.end()); // ISO-8859-1
        else if (data[0] == 1) return std::string(data.begin() + 3, data.end()); // UTF-16, naive
        else return std::string(data.begin(), data.end());
    }

    static int synchsafeToInt(const char* bytes) {
        return ((bytes[0] & 0x7F) << 21) |
               ((bytes[1] & 0x7F) << 14) |
               ((bytes[2] & 0x7F) << 7)  |
               (bytes[3] & 0x7F);
    }
};

#endif //METADATA_HANDLER_H
