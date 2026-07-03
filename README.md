Terminal Music Player

A feature-rich terminal-based music player built in C++ that plays actual audio files using the miniaudio library. No GUI — just your terminal and keyboard controls.

---

Features

· Play, pause, resume, stop, next/previous tracks
· Playback modes: No Repeat, Repeat One, Repeat All, Shuffle
· Switch between read-only playlists (.m3u files)
· Filter songs by Artist or Album
· Search tracks (title/artist/album — case-insensitive)
· Sort by Title, Artist, Album, Year, or Duration
· Persistent settings (settings.cfg) — remembers last played song & playback mode
· Clean TUI with multiple screens (Main Menu, Now Playing, Playlist View, Settings)

---

Setup & Configuration

1. Data Files Structure

The program expects a fixed directory structure:

project/
├── Data/
│   ├── Library.csv     
│   ├── Playlists/
│   │   └── [playlistName.m3u files]
│   └── Musics/
│       └── [.mp3 files]
└── src/
     ├── miniaudio.h          # Provided
     └── [.cpp/.h files]
     
---

2. Library.csv (Master Database)

This file contains all your songs with metadata. The program loads it once at startup.

Format (header required):

title,artist,album,genre,year,duration_sec,file_path
Bohemian Rhapsody,Queen,A Night at the Opera,Rock,1975,354,Data/Musics/bohemian.mp3

· duration_sec: track length in seconds (e.g., 354 for 5:54)
· file_path: relative path to the actual audio file on your system

⚠️ If a row is malformed or a field is missing, the program will skip it and continue.

---

3. Playlist Files (.m3u)

Each .m3u file is a simple list of audio file paths that must match the file_path column in Library.csv.

Format of every line in .m3u file:
Data/Musics/"Artist" - "Song Title".mp3


· All paths in .m3u files are relative (e.g. Data/Musics/Queen - bohemian.mp3) so the project works on any system.
· If a path in .m3u does not exist in Library.csv, it's ignored.
· Playlists are read-only — users cannot add/remove tracks permanently.

---

4. Audio Files

The actual .mp3, .wav, .flac, etc. files must exist at the paths specified in your Library.csv and .m3u files.

Make sure:

· The paths are correct on your machine.
· The audio files are accessible and playable by miniaudio.

---

How to build and run:
# Compile
make clean && make

# Run
./music_player

---
The last played song is shown in the Main Menu (but not auto-played).
The playback mode is restored on next launch.

---
Dependencies:

miniaudio (single-header, included in src/)
C++17 compiler
Linux: pthread library

---
Author: Maryam Jafari
github.com/imaryamjafari
