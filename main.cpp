#include <cstdio>
#include <cstring>
#include <raylib.h>
#include <vector>

typedef struct {
  char *file_path;
  Music music;
} Track;

int main() {
  int screenWidth = 800;
  int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Something");
  InitAudioDevice();
  SetTargetFPS(60);
  SetTraceLogLevel(LOG_ALL);

  float timePlayed = 0.0f;
  bool pause = false;
  std::vector<Track> tracks;
  int current_track = -1;
  Track *cur_track = NULL;

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();
      for (int i = 0; i < droppedFiles.count; i++) {
        char *file_name = strdup(droppedFiles.paths[i]);
        Music music = LoadMusicStream(file_name);
        if (IsMusicReady(music)) {
          tracks.push_back(Track{file_name, music});
        }
      }
      UnloadDroppedFiles(droppedFiles);

      if (tracks.size() > 0) {
        current_track = 0;
        cur_track = &tracks.at(current_track);
        PlayMusicStream(cur_track->music);
      }
    }

    if (tracks.size() > current_track) {
      cur_track = &tracks.at(current_track);
      Music music = cur_track->music;
      float track_length = GetMusicTimeLength(music);
      float track_played = GetMusicTimePlayed(music);
      timePlayed = track_played / track_length;
      if (timePlayed > 1.0f)
        timePlayed = 1.0f;

      if (track_played >= track_length - 0.3) {
        current_track += 1;
        if (current_track < tracks.size()) {
          cur_track = &tracks.at(current_track);
          PlayMusicStream(cur_track->music);
        } else
          StopMusicStream(music);
      }
      UpdateMusicStream(music);

      if (IsKeyPressed(KEY_R)) {
        StopMusicStream(music);
        PlayMusicStream(music);
      }

      if (IsKeyPressed(KEY_SPACE)) {
        if (IsMusicStreamPlaying(music))
          PauseMusicStream(music);
        else
          ResumeMusicStream(music);
      }
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);
    if (tracks.size() > 0) {
      for (int i = 0; i < tracks.size(); i++) {
        const char *file_name = GetFileName(tracks[i].file_path);
        DrawText(file_name, 255, 0 + i * 20, 20, LIGHTGRAY);
      }
    }

    if (cur_track != NULL) {
      const char *file_name = GetFileName(cur_track->file_path);
      DrawText(file_name, 255, 100, 20, LIGHTGRAY);
      DrawText(IsMusicStreamPlaying(cur_track->music) ? "Playing" : "Paused",
               255, 120, 20, LIGHTGRAY);
    }
    DrawRectangle(200, 200, 400, 12, LIGHTGRAY);
    DrawRectangle(200, 200, (int)(timePlayed * 400.0f), 12, MAROON);
    DrawRectangleLines(200, 200, 400, 12, GRAY);
    DrawText("PRESS R TO RESTART MUSIC", 215, 250, 20, LIGHTGRAY);
    DrawText("PRESS SPACE TO PAUSE/RESUME MUSIC", 208, 280, 20, LIGHTGRAY);

    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
  return 0;
}
