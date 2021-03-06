/*
 * Tic-tac-toe
 * MIT Licence, Copyright 2017 Chris Kempson (chriskempson.com)
 */

#include <iostream>
#include <string>
#include "Grid.h"
#include "Setting.h"
#include "Resource_manager.h"

//these are variables that are to be used in the score board
int red_count_wins = 0;
int yellow_count_wins = 0;
int count_draws = 0;
int num;

void Grid::init()
{
	// asking user to confirm whether they want the orignal set of sprites or the new sprites
	std::cout << std::endl << "Again for confirmation? (1 or 2)" << std::endl;
	std::cin >> num;

	// Setup sprite clipping rects
	for (int i = 0; i < 3; i++) {
		sprite_clips[i].x = 0;
		sprite_clips[i].y = i * Setting::grid_sprite_height;
		sprite_clips[i].w = Setting::grid_sprite_width;
		sprite_clips[i].h = Setting::grid_sprite_height;
	}
}

/**
 * The grid is comprised of a single repeated sprite. The sprite contains an
 * image for a blank cell, a cell with a red nought and a cell with a yellow
 * cross.
 */
void Grid::render()
{
	if(num == 1)
	{
		// Iterate over columns and rows
	for (int col = 0; col < Setting::grid_columns; col++) {
		for (int row = 0; row < Setting::grid_rows; row++) {

			// Position sprites in grid layout
			Resource_manager::get_image("cell_sprite")->set_position(col * Setting::grid_sprite_width, row  * Setting::grid_sprite_height);

			// Render sprite with current clipping rectangle
			Resource_manager::get_image("cell_sprite")->render(&sprite_clips[cell[col][row].current_sprite]);
		}
	}
	}
	else if (num == 2)
	{
		// Iterate over columns and rows
		for (int col = 0; col < Setting::grid_columns; col++) {
			for (int row = 0; row < Setting::grid_rows; row++) {

				// Position sprites in grid layout
				Resource_manager::get_image("cell_sprite1")->set_position(col * Setting::grid_sprite_width, row  * Setting::grid_sprite_height);

				// Render sprite with current clipping rectangle
				Resource_manager::get_image("cell_sprite1")->render(&sprite_clips[cell[col][row].current_sprite]);
			}
		}
	}
}

/**
* Clears the grid by changing all of the cell sprites to sprite_blank
*/
void Grid::clear()
{
	// Iterate over columns and rows
	for (int col = 0; col < Setting::grid_columns; col++) {
		for (int row = 0; row < Setting::grid_rows; row++) {

			// Set the cell sprite to sprite_blank
			cell[col][row].current_sprite = sprite_blank;
		}
	}
}

const char* Grid::get_player_name(int sprite)
{
	// Since the colors used are red and blue, on the bottom it will also keep track of the color
	//associated with the user
	static const char* player_names[] = { "Red", "Blue" };

	// Return a player name
	return player_names[sprite -1];

}

// this function was made in order to be able to take in the name from the users
// and eventually use it in the scoreboard to display the winners name
// was necessary because without it, the scoreboard would only display an increase
//in draws because the red win count and blue win count depended on the above function
const std::string Grid::get_player_input(int sprite)
{
	// player one by default is red and player two is blue by default
	if (sprite == 1)
	{
		return playerOneName;
	}
	else {
		return playerTwoName;
	}
}

bool Grid::check_for_draw()
{
	// Iterate over columns and rows
	for (int col = 0; col < Setting::grid_columns; col++) {
		for (int row = 0; row < Setting::grid_rows; row++) {

			// If any of the cells have a sprite_blank clipping displayed
			// there can be no draw
			if (cell[col][row].current_sprite == sprite_blank) {
				return false;
			}
		}
	}

	std::cout << std::endl << "******** DRAW *********" << std::endl;
	std::cout << "Draw for both players" << std::endl;
	count_wins(sprite_blank);

	return true;
}

bool Grid::check_for_win(int col, int row, sprite_sheet sprite)
{
	if (check_for_row_win(col, row, sprite)) {
		return true;
	}
	else if (check_for_column_win(col, row, sprite)) {
		return true;
	}
	else if (check_for_forwards_diagonal_win(col, row, sprite)) {
		return true;
	}
	else if (check_for_backwards_diagonal_win(col, row, sprite)) {
		return true;
	}

	return false;
}

bool Grid::check_for_row_win(int col, int row, sprite_sheet sprite)
{
	// How many times a particular sprite clipping is matched
	int consecutive_matches = 0;

	// Iterate over columns in a specified row. For example:
	// [X][X][X]
	// [ ][ ][ ]
	// [ ][ ][ ]
	for (int i = 0; i < Setting::grid_columns; i++) {

		// Count consecutive matches
		if (cell[i][row].current_sprite == sprite) {
			consecutive_matches++;
		}
		else {
			consecutive_matches = 0;
		}

		// If there are enough consecutive matches return true for a win
		if (consecutive_matches == Setting::win_count) {
			std::cout << std::endl << "******** WINNER *********" << std::endl;
			std::cout << "Row win for " << get_player_input(sprite) << std::endl;
			count_wins(sprite);

			return true;
		}
	}

	return false;
}

bool Grid::check_for_column_win(int col, int row, sprite_sheet sprite)
{
	// How many times a particular sprite clipping is matched
	int consecutive_matches = 0;

	// Iterate over rows in a specified column. For example:
	// [X][ ][ ]
	// [X][ ][ ]
	// [X][ ][ ]
	for (int i = 0; i < Setting::grid_rows; i++) {

		// Count consecutive matches
		if (cell[col][i].current_sprite == sprite) {
			consecutive_matches++;
		}
		else {
			consecutive_matches = 0;
		}

		// If there are enough consecutive matches return true for a win
		if (consecutive_matches == Setting::win_count) {
			std::cout << std::endl << "******** WINNER *********" << std::endl;
			std::cout << "Column win for " << get_player_input(sprite) << std::endl;
			count_wins(sprite);

			return true;
		}
	}

	return false;
}

bool Grid::check_for_forwards_diagonal_win(int col, int row, sprite_sheet sprite)
{
	// How many times a particular sprite clipping is matched
	int consecutive_matches = 0;

	// Check for Setting::win_count * 2 times for the reason explained below
	for (int i = 0; i < Setting::win_count * 2; i++) {

		// Count from the bottom of the forwards diagonal to the top. For
		// example, on a 3x3 grid where the move made is marked by "4" the
		// check will run in the direction of 1 to 6 starting outside of the
		// grid as shown below:
		//
		//       COL 0  1  2
		//          [ ][ ][6] ROW 0
		//          [ ][5][ ] ROW 1
		//          [4][ ][ ] ROW 2
		//       [3]
		//    [2]
		// [1]
		//
		// If the move made is marked by "3". The search pattern would look
		// like the following:
		//
		//                         [6]
		//                      [5]
		//       COL 0  1  2 [4]
		//          [ ][ ][3] ROW 0
		//          [ ][2][ ] ROW 1
		//          [1][ ][ ] ROW 2
		//
		int check_col = col + i - Setting::win_count;
		int check_row = row - i + Setting::win_count;

		// Only check when coords are valid otherwise we get false positives
		if ((check_col >= 0 && check_col < Setting::grid_columns) && (check_row >= 0 && check_row < Setting::grid_rows)) {

			// Count consecutive matches
			if (cell[check_col][check_row].current_sprite == sprite) {
				consecutive_matches++;
			}
			else {
				consecutive_matches = 0;
			}

			// If there are enough consecutive matches return true for a win
			if (consecutive_matches == Setting::win_count) {
				std::cout << std::endl << "******** WINNER *********" << std::endl;
				std::cout << "Forwards diagonal (/) win for " << get_player_input(sprite) << std::endl;
				count_wins(sprite);

				return true;
			}
		}
	}

	return false;
}

bool Grid::check_for_backwards_diagonal_win(int col, int row, sprite_sheet sprite)
{
	// How many times a particular sprite clipping is matched
	int consecutive_matches = 0;

	// Check for Setting::win_count * 2 times for the reason explained below
	for (int i = 0; i < Setting::win_count * 2; i++) {

		// Count from the top of the backwards diagonal to the bottom. For
		// example, on a 3x3 grid where the move made is marked by "5" the
		// check will run in the direction of 1 to 6 starting outside of the
		// grid as shown below:
		//
		// [1]
		//    [2]
		//       [3] 0  1  2
		//          [4][ ][ ] ROW 0
		//          [ ][5][ ] ROW 1
		//          [ ][ ][6] ROW 2
		//
		// If the move made is marked by "3". The search pattern would look
		// like the following:
		//
		//
		//
		//           0  1  2
		//          [1][ ][ ] ROW 0
		//          [ ][2][ ] ROW 1
		//          [ ][ ][3] ROW 2
		//                   [4]
		//                      [5]
		//                         [6]
		//
		int check_col = col + i - Setting::win_count;
		int check_row = row + i - Setting::win_count;

		// Only check when coords are valid otherwise we get false positives
		if ((check_col >= 0 && check_col < Setting::grid_columns) && (check_row >= 0 && check_row < Setting::grid_rows)) {

			// Count consecutive matches
			if (cell[check_col][check_row].current_sprite == sprite) {
				consecutive_matches++;
			}
			else {
				consecutive_matches = 0;
			}

			// If there are enough consecutive matches return true for a win
			if (consecutive_matches == Setting::win_count) {
				std::cout << std::endl << "******** WINNER *********" << std::endl;
				std::cout << "Backwards diagonal (\\) win for " << get_player_input(sprite) << std::endl;
				count_wins(sprite);

				return true;
			}
		}
	}

	return false;
}
void Grid::count_wins(sprite_sheet sprite)
{
	if (get_player_name(sprite) == "Red")
	{
		red_count_wins++;

		std::cout << std::endl;
		std::cout << "Red wins: " << red_count_wins << std::endl;
		std::cout << "Blue wins: " << yellow_count_wins << std::endl;
		std::cout << "Draws: " << count_draws << std::endl;
	}
	else if (get_player_name(sprite) == "Blue")
	{
		yellow_count_wins++;

		std::cout << std::endl;
		std::cout << "Red wins: " << red_count_wins << std::endl;
		std::cout << "Blue wins: " << yellow_count_wins << std::endl;
		std::cout << "Draws: " << count_draws << std::endl;
	}
	else
	{
		count_draws++;

		std::cout << std::endl;
		std::cout << "Red wins: " << red_count_wins << std::endl;
		std::cout << "Blue wins: " << yellow_count_wins << std::endl;
		std::cout << "Draws: " << count_draws << std::endl;
	}
}
