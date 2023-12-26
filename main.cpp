#include <raylib.h>
#include <string>
#include <vector>

typedef struct Track {
  std::string file_name;
  Music music;
} Track;

typedef struct PlayerState {
  std::vector<Track> tracks;
  int current_track;
} PlayerState;

int main() {
  int screenWidth = 800;
  int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Something");
  InitAudioDevice();
  SetTargetFPS(60);

  float timePlayed = 0.0f;
  bool pause = false;
  PlayerState player_state = PlayerState();
  player_state.current_track = -1;

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();
      if (droppedFiles.count == 1) {
        Music music = LoadMusicStream(droppedFiles.paths[0]);
        std::string file_name = droppedFiles.paths[0];
        player_state.tracks.push_back(Track{file_name, music});
        player_state.current_track = 0;
      }
      UnloadDroppedFiles(droppedFiles);
    }

    if (player_state.current_track >= 0 &&
        IsMusicReady(player_state.tracks[player_state.current_track].music)) {
      Music cur_music = player_state.tracks[player_state.current_track].music;
      PlayMusicStream(cur_music);
      UpdateMusicStream(cur_music);

      if (IsKeyPressed(KEY_R)) {
        StopMusicStream(cur_music);
        PlayMusicStream(cur_music);
      }

      if (IsKeyPressed(KEY_SPACE)) {
        pause = !pause;
        if (pause)
          PauseMusicStream(cur_music);
        else
          ResumeMusicStream(cur_music);
      }

      timePlayed =
          GetMusicTimePlayed(cur_music) / GetMusicTimeLength(cur_music);
      if (timePlayed > 1.0f)
        timePlayed = 1.0f;

      if (timePlayed >= GetMusicTimeLength(cur_music)) {
        player_state.current_track = -1;
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
