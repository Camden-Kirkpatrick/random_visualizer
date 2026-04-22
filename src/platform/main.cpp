#include <iostream>
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>
#include <cstdint>
#include "imguiThemes.hpp"
#include "gameMain.hpp"



int main()
{
#if PRODUCTION_BUILD == 1
	SetTraceLogLevel(LOG_NONE); // Don't open the console for the production build
#endif

	// --- Window and raylib setup ---
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WIN_WIDTH, WIN_HEIGHT, "game");
	SetTargetFPS(FPS);

#pragma region imgui
	// --- ImGui setup (runs once before the main loop) ---
	rlImGuiSetup(true);

	// Allow docking and increase font size
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontGlobalScale = 2;

	// Change the style/theme
	//ImGui::StyleColorsClassic();
	ImGui::SetupImGuiCatppuccinMochaStyle();
#pragma endregion

	// Populate the grid for the first frame
	if (!initGame())
	{
		return 1;
	}

	// --- Main loop: runs once per frame until the window is closed ---
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

	#pragma region imgui
		rlImGuiBegin();

		// Allow ImGui windows to dock to the game window
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);

		// --- The control panel window ---
		ImGui::Begin("control panel");

		// Randomize: re-fill the grid using the current RNG state.
		// Each press advances the RNG further, so the result is different every time.
		if (ImGui::Button("Randomize"))
			initGame();

		ImGui::Separator();

		// Seed input + Regenerate:
		// Setting a seed and pressing Regenerate gives the same image every time for that seed.
		static uint32_t seed = 42;
		ImGui::InputScalar("Seed", ImGuiDataType_U32, &seed);
		if (ImGui::Button("Regenerate"))
		{
			setState(seed); // Rewind the RNG back to this seed
			initGame();     // Regenerate using that known starting point
		}

		ImGui::Separator();

		// --- Color pickers for the two grid states (1 and 0) ---
		// Colors for the 1 and 0 cells
		static Color colors[2] = { WHITE, BLACK };

		// ColorEdit3 uses floats [0.0, 1.0], so convert from raylib's [0, 255]
		float c0[4] = { colors[0].r / 255.0f, colors[0].g / 255.0f, colors[0].b / 255.0f, 1 };
		float c1[4] = { colors[1].r / 255.0f, colors[1].g / 255.0f, colors[1].b / 255.0f, 1 };

		ImGui::ColorEdit3("Color #1", c0);
		// write back in case ImGui changed c0
		colors[0] = { (uint8_t)(c0[0] * 255), (uint8_t)(c0[1] * 255), (uint8_t)(c0[2] * 255), 255 };
		ImGui::ColorEdit3("Color #2", c1);
		// write back in case ImGui changed c1
		colors[1] = { (uint8_t)(c1[0] * 255), (uint8_t)(c1[1] * 255), (uint8_t)(c1[2] * 255), 255 };

		// Restore the default black-and-white palette
		if (ImGui::Button("Reset color"))
			setColors(WHITE, BLACK);

		// Apply the currently-picked colors to the grid
		if (ImGui::Button("Change color"))
			setColors(colors[0], colors[1]);

		ImGui::Separator();

		// Cell size control — bigger cells = fewer, chunkier rectangles.
		// Changing it triggers a resize of the matrix and a regeneration.
		if (ImGui::InputScalar("Cell size", ImGuiDataType_U32, &cellSize))
		{
			if (cellSize < 1) cellSize = 1; // Avoid division by zero in resize()
			resize();
			initGame();
		}


		ImGui::Separator();

		// Animation toggle — when on, the grid re-randomizes every frameInterval frames
		ImGui::Checkbox("Animate", &animate);

		// How many frames between each animation update (lower = faster animation)
		static uint16_t min = 1;
		static uint16_t max = 500;
		ImGui::SliderScalar("Interval in frames", ImGuiDataType_U16, &frameInterval, &min, &max);
		if (frameInterval < 1) frameInterval = 1; // Guard against division by zero

		ImGui::Separator();

		// Choose which random number generator drives the grid
		ImGui::RadioButton("Good random number generator", &generatorSelection , 1);
		ImGui::SameLine();
		ImGui::RadioButton("Bad random number generator", &generatorSelection, 2);
		ImGui::RadioButton("Color percentage", &generatorSelection, 3);
		ImGui::SliderFloat("Percentage", &colorPercentage, 0.0f, 1.0f);

		ImGui::Separator();

		// Which bit of the bad RNG's output to extract.
		// Low bits (0-5) show strong visible patterns; higher bits look more random.
		ImGui::InputScalar("Extraction bit", ImGuiDataType_U32, &extractBit);

		ImGui::Separator();

		// One-click return to the initial defaults
		if (ImGui::Button("Reset All"))
		{
			setState(42);
			setColors(WHITE, BLACK);
			cellSize = 10;
			frameInterval = 60;
			animate = false;
			generatorSelection = 1;
			colorPercentage = 0.5f;
			extractBit = 8;
			initGame();
		}


		ImGui::End();
	#pragma endregion


		// Draws the grid and advances animation
		if (!updateGame())
		{
			CloseWindow();
		}

	#pragma region imgui
		rlImGuiEnd();
	#pragma endregion

		EndDrawing();
	}

#pragma region imgui
	rlImGuiShutdown();
#pragma endregion

	CloseWindow();

	closeGame(); // Write shutdown marker to file

	return 0;
}
