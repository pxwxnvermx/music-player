#include <raylib.h>
#include <vector>

typedef struct {
  char *file_path;
  Music music;
} Track;

typedef struct {
  std::vector<Track> tracks;
  int current_track;
} PlayerState;

int main() {
  int screenWidth = 800;
  int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Something");
  InitAudioDevice();
  SetTargetFPS(60);
  SetTraceLogLevel(LOG_ALL);

  float timePlayed = 0.0f;
  bool pause = false;
  PlayerState player_state = PlayerState();
  player_state.current_track = -1;

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();
      for (int i = 0; i < droppedFiles.count; i++) {
        char *file_name = droppedFiles.paths[i];
        Music music = LoadMusicStream(file_name);
        if (IsMusicReady(music)) {
          player_state.tracks.push_back(Track{file_name, music});
        }
      }
      UnloadDroppedFiles(droppedFiles);

      if (player_state.tracks.size() > 0) {
        player_state.current_track = 0;
        Track cur_track = player_state.tracks.at(player_state.current_track);
        PlayMusicStream(cur_track.music);
      }
    }

    if (player_state.current_track >= 0) {
      Track cur_track = player_state.tracks.at(player_state.current_track);
      Music cur_music = cur_track.music;

      float track_length = GetMusicTimeLength(cur_music);
      float track_played = GetMusicTimePlayed(cur_music);
      timePlayed = track_played / track_length;
      if (timePlayed > 1.0f)
        timePlayed = 1.0f;

      if (track_played >= track_length - 0.3) {
        if (player_state.current_track < player_state.tracks.size()) {
          player_state.current_track += 1;
          cur_track = player_state.tracks.at(player_state.current_track);
          PlayMusicStream(cur_track.music);
        } else
          StopMusicStream(cur_music);
      }

      UpdateMusicStream(cur_music);

      if (IsKeyPressed(KEY_R)) {
        StopMusicStream(cur_music);
        PlayMusicStream(cur_music);
      }

      if (IsKeyPressed(KEY_SPACE)) {
        if (IsMusicStreamPlaying(cur_music))
          PauseMusicStream(cur_music);
        else
          ResumeMusicStream(cur_music);
      }
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawText("MUSIC SHOULD BE PLAYING!", 255, 150, 20, LIGHTGRAY);
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
