#include <iostream>
#include <cstdlib> // For system()
#include <vector>
#include <filesystem>
#include <cstring> // getenv()
#include <algorithm> // For shuffle
#include <random> // For random device

using namespace std;

void clearScreen() {
    cout << "\033[2J\033[H"; // ANSI escape code to clear screen
}

bool mpgInstallCheck() {
    return system("command -v mpg123 > /dev/null 2>&1") == 0;
}

void installMpg() {
    char response;
    cout << "mpg123 is not installed. Would you like to install it now? (y/n): ";
    cin >> response;

    if (response == 'y' || response == 'Y') {
        system("sudo pacman -S mpg123");
    } else {
        cerr << "mpg123 is required for playing MP3 Files.";
        exit(1);
    }
}

void playSongsInOrder(const vector<string>& mp3Files) {
    for (const auto& file : mp3Files) {
        clearScreen();
        cout << "Now playing: " << filesystem::path(file).filename().string() << "\n\n";
        string command = "mpg123 -q --no-control \"" + file + "\"";
        int result = system(command.c_str());
        if (result != 0) {
            cerr << "Error playing the file: " << file << "\n";
        }
    }
}

void playSongsRandomly(vector<string> mp3Files) {
    random_device rd;
    mt19937 g(rd());
    shuffle(mp3Files.begin(), mp3Files.end(), g);

    playSongsInOrder(mp3Files);
}

void menu(const vector<string>& mp3Files) {
    while (true) {
        string buffer;
        system("clear");
        cout << "The Best MP3 Player:\n";
        cout << "1. Select an MP3 to play\n";
        cout << "2. Play all songs in order\n";
        cout << "3. Play all songs randomly\n";
        cout << "4. Open mpg123 menu\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            system("clear");
            cout << "Your Music\n";
            for (size_t i = 0; i < mp3Files.size(); ++i) {
                filesystem::path p(mp3Files[i]);
                cout << i + 1 << ". " << p.filename().string() << "\n";
            }
            cout << "Enter the number of the MP3 file to play: ";
            int fileChoice;
            cin >> fileChoice;

            if (fileChoice < 1 || fileChoice > static_cast<int>(mp3Files.size())) {
                cerr << "Invalid choice. Try again.\n";
                cin.ignore();
                cin.get();
            } else {
                system("clear");
                filesystem::path p(mp3Files[fileChoice - 1]);
                cout << "Now playing: " << p.filename().string() << "\n\n";
                string command = "mpg123 -q --no-control \"" + mp3Files[fileChoice - 1] + "\"";
                int result = system(command.c_str());
                
                if (result != 0) {
                    cerr << "Error playing the file.\n";
                }
                cin.ignore();
                cin.get();
            }
        } else if (choice == 2) {
            playSongsInOrder(mp3Files);
        } else if (choice == 3) {
            playSongsRandomly(mp3Files);
        } else if (choice == 4) {
            system("clear");
            system("mpg123 -?");
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        } else if (choice == 5) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid option. Try again.\n";
            cin.ignore();
            cin.get();
        }
    }
}

int main() {
    if (!mpgInstallCheck()) {  // Fixed logic here
        installMpg();
    }

    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        cerr << "Could not find home directory.\n";
        return 1;
    }

    string musicDir = string(homeDir) + "/Music";
    filesystem::create_directories(musicDir);

    string playlistcDir = musicDir + "/playlistc";
    filesystem::create_directories(playlistcDir);

    vector<string> mp3Files;
    for (const auto& entry : filesystem::directory_iterator(playlistcDir)) {
        if (entry.path().extension() == ".mp3") {
            mp3Files.push_back(entry.path().string());
        }
    }

    if (mp3Files.empty()) {
        cout << "No MP3 files found in " << playlistcDir << "\n";
        return 0;
    }

    menu(mp3Files);
    return 0;
}
