#include <raylib.h> // Include the Raylib library for graphics and input
#include <iostream> // Include the iostream library for input/output operations
#include <deque>    // Include the deque header file for using deque data structure
#include <raymath.h>    // Include the raymath header file for using raylib's math functions

Color green = { 173, 204, 96, 255 };    // Define a color named 'green' with specified RGBA values
Color darkGreen = { 43, 51, 24, 255 };  // Define another color named 'darkGreen' with different RGBA values

int cellSize = 30;  // Set the size of each cell in the grid
int cellCount = 25; // Set the total number of cells in the grid
int offset = 75;    // Define an offset value to adjust positions or coordinates in the grid

double lastUpdateTime = 0;  // Initialize a variable to store the time of the last update

// This function checks if a given Vector2 element exists in a deque container.
bool ElementInDeque(Vector2 element, std::deque<Vector2> deque)
{   
    // Iterate through each element in the deque container
    for (size_t i = 0; i < deque.size(); i++)
    {
        // Check if the current element in the deque is equal to the target element
        if (Vector2Equals(deque[i], element))
        {
            // If a match is found, return true to indicate that the element exists in the deque
            return true;
        }
    }
    return false;   // If no match is found after iterating through the entire deque, return false
}

// This function checks if a certain time interval has elapsed since the last event update.
bool EventTriggered(double interval)
{
    double currentTime = GetTime(); // Get the current time in seconds.

    // Check if the difference between the current time and the time of the last update
    // is greater than or equal to the specified interval
    if (currentTime - lastUpdateTime >= interval)
    {
        // If the condition is true, update the last update time to the current time
        lastUpdateTime = currentTime;
        // Return true to indicate that the event should be triggered
        return true;
    }
    return false;   // If the condition is false, return false to indicate that the event should not be triggered
}

class Snake
{
public:
    std::deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} }; // Define the snake's initial body position
    Vector2 direction = { 1, 0 };   // Set the initial direction of the snake
    bool addSegment = false;    // Flag to determine whether to add a new segment to the snake's body

    void Draw() // Method to draw the snake on the screen
    {
        for (size_t i = 0; i < body.size(); i++)    // Iterate over each segment of the snake's body
        {
            float x = body[i].x;    // Get the x-coordinate of the current segment
            float y = body[i].y;    // Get the y-coordinate of the current segment
            Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};  // Define a rectangle representing the current segment's position and size
            DrawRectangleRounded(segment, 0.5f, 6.0f, darkGreen);   // Draw the current segment on the screen
        }
    }

    void Update()   // Method to update the snake's position
    {
        body.push_front(Vector2Add(body[0], direction));    // Move the snake's head in the current direction
        if (addSegment == true) // If a new segment is flagged to be added
        {            
            addSegment = false; // Reset the flag
        }
        else
        {
            body.pop_back();    // Remove the last segment of the snake's body            
        }        
    }

    void Reset()    // Method to reset the snake's position and direction
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} }; // Reset the snake's body to its initial position
        direction = { 1, 0 };   // Reset the snake's direction to its initial direction
    }
};

class Food  // Class to represent the food in the game
{
public:
    Vector2 position;   // Position of the food    
    Texture2D texture;   // Texture of the food image

    Food(std::deque<Vector2> snakeBody)  // Constructor of the Food class
    {
        Image image = LoadImage("Graphics/food.png");   // Load the food image from the file
        texture = LoadTextureFromImage(image);  // Create a texture from the image
        UnloadImage(image); // Unload the image from memory

        position = GenerateRandomPosition(snakeBody);   // Generate a random position for the food
    }

    ~Food() // Destructor of the Food class
    {
        UnloadTexture(texture); // Unload the texture from memory
    }

    void Draw() // Function to draw the food on the screen
    {
        DrawTexture(texture, offset + position.x*cellSize, offset + position.y*cellSize, WHITE);  // Draw the texture at the position with white color
    }

    Vector2 GenerateRandomCell()    // Function to generate a random cell position
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        Vector2 position = { x, y };

        return Vector2{ x, y };
    }

    Vector2 GenerateRandomPosition(std::deque<Vector2> snakeBody)   // Function to generate a random position for the food
    {
        Vector2 position = GenerateRandomCell();    // Generate a random cell position

        // Ensure that the food position is not overlapping with the snake body
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();    // Generate another random cell position
        }

        return position;    // Return the valid food position
    }
};

class Game
{
public:
    Snake snake = Snake();          // Instance of the Snake class representing the game's snake
    Food food = Food(snake.body);   // Instance of the Food class representing the game's food, initialized with the snake's body
    bool running = true;            // Flag indicating whether the game is running
    int score = 0;                  // Current score of the player
    Sound eatSound;                 // Sound effect for when the snake eats food
    Sound wallSound;                // Sound effect for when the snake hits the wall

    Game()
    {
        InitAudioDevice();          // Initialize the audio device
        eatSound = LoadSound("Sounds/eat.mp3");   // Load the eat sound effect
        wallSound = LoadSound("Sounds/wall.mp3"); // Load the wall collision sound effect
    }

    ~Game()
    {
        UnloadSound(eatSound);      // Unload the eat sound effect
        UnloadSound(wallSound);     // Unload the wall collision sound effect
        CloseAudioDevice();         // Close the audio device
    }

    void Draw()
    {
        food.Draw();                // Draw the food on the screen
        snake.Draw();               // Draw the snake on the screen
    }

    void Update()
    {
        if (running == true)
        {
            snake.Update();                 // Update the snake's position and state
            CheckCollisionWithFood();       // Check for collision with food
            CheckCollisionWithEdges();      // Check for collision with edges of the screen
            CheckCollisionWithTail();       // Check for collision with the snake's tail
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPosition(snake.body);  // Generate new position for food
            snake.addSegment = true;                                  // Inform snake to add a new segment
            score++;                                                  // Increment the score
            PlaySound(eatSound);                                      // Play the eat sound effect
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();                // Game over if snake hits horizontal edges
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();                // Game over if snake hits vertical edges
        }
    }

    void GameOver()
    {
        snake.Reset();                // Reset the snake's position and state
        food.position = food.GenerateRandomPosition(snake.body);  // Generate new position for food
        running = false;              // Set the game state to not running
        score = 0;                    // Reset the score
        PlaySound(wallSound);         // Play the wall collision sound effect
    }

    void CheckCollisionWithTail()
    {
        std::deque<Vector2> headlessBody = snake.body;   // Copy of the snake's body without the head
        headlessBody.pop_front();                        // Remove the head from the copy
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();               // Game over if snake collides with its own tail
        }
    }
};

int main()
{
    std::cout << "Game Commence\n";  // Print a message to the console indicating game start

    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");  // Create the game window with appropriate size and title
    SetTargetFPS(60);  // Set the target frames per second for smooth animation

    Game game = Game();  // Create an instance of the Game class

    while (WindowShouldClose() == false)  // Main game loop (runs until the window is closed)
    {
        BeginDrawing();  // Begin drawing operations

        if (EventTriggered(0.2))  // Check if the event is triggered (e.g., for updating the game)
        {
            game.Update();  // Update the game logic
        }

        // Check for key presses to change the snake's direction
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = { -1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }

        ClearBackground(green);  // Clear the screen with the light green background color
        DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5.0f, darkGreen);  // Draw the border of the game area
        DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);  // Draw the game title
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);  // Draw the player's score
        game.Draw();  // Draw the game elements (snake and food)

        EndDrawing();  // End drawing operations
    }

    CloseWindow();  // Close the game window

    return 0;
}

