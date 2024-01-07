#pragma once

#include <string>
#include <vector>

class StubMidiService {
public:
    std::vector<std::string> messages;

    StubMidiService(int midiPort) {}

    void openMidiPort(int midiPort) {}

    void noteOn(int channel, int note, int velocity) {
        messages.push_back(
            "noteOn " + std::to_string(channel)
            + " " + std::to_string(note)
            + " " + std::to_string(velocity)
        );
    }

    void noteOff(int channel, int note) {
        messages.push_back(
            "noteOff " + std::to_string(channel) + " "  + std::to_string(note)
        );
    }

    void cc(int channel, int controller, int value) {
        messages.push_back(
            "cc " + std::to_string(channel)
            + " " + std::to_string(controller)
            + " " + std::to_string(value)
        );
    }

    void printMidiOutputDevices() {}

    int getMessagesSize() {
        return messages.size();
    }

    std::string& getMessage(int idx) {
        return messages[idx];
    }
};
