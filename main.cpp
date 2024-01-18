#include <cstring>
#include <raylib.h>
#include <string>
#include <vector>

typedef struct {
  std::string file_path;
  Music music;
} Track;

bool DrawButton(Vector2 bounds, const char *text, Color color) {
  bool result = false;
  Rectangle rect_bounds = Rectangle{.x = bounds.x, .y = bounds.y};
  Rectangle text_bounds = Rectangle{.x = bounds.x, .y = bounds.y};
  text_bounds.width = MeasureText(text, 20);
  text_bounds.height = 20;
  rect_bounds.width = text_bounds.width + 20;
  rect_bounds.height = text_bounds.height + 10;
  text_bounds.x += (rect_bounds.width - text_bounds.width) * 0.5;
  text_bounds.y += (rect_bounds.height - text_bounds.height) * 0.5;

  Vector2 mousePoint = GetMousePosition();
  if (CheckCollisionPointRec(mousePoint, rect_bounds)) {
    Color dark_color = ColorBrightness(color, 0.1f);
    DrawRectangle(rect_bounds.x, rect_bounds.y + rect_bounds.height,
                  rect_bounds.width + 2, 2, dark_color);
    DrawRectangle(rect_bounds.x + rect_bounds.width, rect_bounds.y, 2,
                  rect_bounds.height + 2, dark_color);
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
      result = true;
  }
  DrawRectangleLinesEx(rect_bounds, 2, color);
  DrawText(text, text_bounds.x, text_bounds.y, 20, color);
  return result;
}

// pp_music = Play Pause music
bool pp_music(Music music) {
  bool music_playing = IsMusicStreamPlaying(music);
  music_playing ? PauseMusicStream(music) : ResumeMusicStream(music);
  return music_playing;
}

int change_track(std::vector<Track> *tracks, int current_track,
                 int changed_track) {
  Track cur_track = tracks->at(current_track);
  StopMusicStream(cur_track.music);
  if (0 <= changed_track && changed_track < tracks->size()) {
    cur_track = tracks->at(changed_track);
    PlayMusicStream(cur_track.music);
    return changed_track;
  }
  return current_track;
}

int main() {
  int factor = 80;
  int screenWidth = factor * 16;
  int screenHeight = factor * 9;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Music Player");
  InitAudioDevice();
  SetTargetFPS(60);
  SetTraceLogLevel(LOG_ALL);

  std::vector<Track> tracks;
  int current_track = -1;
  Track *cur_track = NULL;

  const Color BG_COL = LIGHTGRAY;
  const Color TEXT_COL = BLACK;
  std::string pp_btn_text = "PAUSED";
  float timePlayed = 0.0f;

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();
      for (int i = 0; i < droppedFiles.count; i++) {
        std::string file_name = strdup(droppedFiles.paths[i]);
        Music music = LoadMusicStream(file_name.c_str());
        if (IsMusicReady(music)) {
          tracks.push_back(Track{file_name, music});
        }
      }
      UnloadDroppedFiles(droppedFiles);

      if (tracks.size() > 0 && current_track == -1) {
        current_track = 0;
        cur_track = &tracks.at(current_track);
        PlayMusicStream(cur_track->music);
      }
    }

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    if (cur_track != NULL) {
      cur_track = &tracks.at(current_track);
      Music music = cur_track->music;
      float track_length = GetMusicTimeLength(music);
      float track_played = GetMusicTimePlayed(music);
      timePlayed = track_played / track_length;
      if (timePlayed > 1.0f)
        timePlayed = 1.0f;

      if (track_played >= track_length - 0.3)
        current_track = change_track(&tracks, current_track, current_track + 1);

      UpdateMusicStream(music);

      if (IsKeyPressed(KEY_R)) {
        StopMusicStream(music);
        PlayMusicStream(music);
      }

      if (IsKeyPressed(KEY_SPACE))
        pp_music(music);
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (tracks.size() > 0) {
      for (int i = 0; i < tracks.size(); i++) {
        if (i == current_track) {
          DrawText(GetFileName(tracks[i].file_path.c_str()), 10, 0 + i * 20, 20,
                   TEXT_COL);
        } else
          DrawText(GetFileName(tracks[i].file_path.c_str()), 10, 0 + i * 20, 20,
                   BG_COL);
      }
    }

    // PREV 76
    // PAUSED 102
    // NEXT 76
    // 262 full width of controls with padding for first two button
    float controls_x = (screenWidth - 262) * 0.5;
    float controls_y = (screenHeight - 50);
    bool prev_btn_clicked =
        DrawButton(Vector2{.x = controls_x, .y = controls_y}, "PREV", BG_COL);
    bool pp_btn_clicked =
        DrawButton(Vector2{.x = controls_x + 80, .y = controls_y},
                   pp_btn_text.c_str(), BG_COL);
    bool next_btn_clicked = DrawButton(
        Vector2{.x = controls_x + 186, .y = controls_y}, "NEXT", BG_COL);

    if (cur_track != NULL) {
      const char *file_name = GetFileName(cur_track->file_path.c_str());
      float file_name_width = MeasureText(file_name, 20);
      DrawText(file_name, (screenWidth - file_name_width) * 0.5,
               controls_y - 25, 20, TEXT_COL);

      if (pp_btn_clicked) {
        if (pp_music(cur_track->music))
          pp_btn_text = "PLAYING";
        else
          pp_btn_text = "PAUSED";
      }

      if (prev_btn_clicked)
        current_track = change_track(&tracks, current_track, current_track - 1);

      if (next_btn_clicked)
        current_track = change_track(&tracks, current_track, current_track + 1);
    }

    DrawRectangle((screenWidth - 400) * 0.5, controls_y - 60, 400, 12, BG_COL);
    DrawRectangle((screenWidth - 400) * 0.5, controls_y - 60,
                  (int)(timePlayed * 400.0f), 12, MAROON);
    DrawRectangleLines((screenWidth - 400) * 0.5, controls_y - 60, 400, 12,
                       GRAY);
    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
  return 0;
}
