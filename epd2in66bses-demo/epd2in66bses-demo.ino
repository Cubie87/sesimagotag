/*****************************************************************************
* E-Ink Burn-In / Ghosting Cleaner
*
* Based on:
*   Waveshare 2.66" BWR SES VUSION demo
*
* Supported commands over Serial @ 115200 baud:
*
*   refresh screen
*       Run the burn-in / ghosting cleaner.
*       Prompts for 10, 25, 50, or 100 cycles.
*
*   write image
*       Display gImage_2in66bb from ImageData.c
*
*   help
*       Show available commands.
*
*   status
*       Show display status.
*
*****************************************************************************/

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ImageData.h"

#include <stdlib.h>
#include <string.h>


// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

#define SERIAL_BAUD 115200

#define REFRESH_DELAY_MS 3000
#define CLEAR_DELAY_MS   500

#define SERIAL_BUFFER_SIZE 64


// ---------------------------------------------------------------------------
// Framebuffers
// ---------------------------------------------------------------------------

UBYTE *BlackImage = NULL;
UBYTE *RedImage   = NULL;

UWORD ImageSize = 0;


// ---------------------------------------------------------------------------
// Display state
// ---------------------------------------------------------------------------

bool displayInitialised = false;


// ---------------------------------------------------------------------------
// Serial command buffer
// ---------------------------------------------------------------------------

char serialBuffer[SERIAL_BUFFER_SIZE];
uint8_t serialBufferIndex = 0;


// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------

bool allocateFramebuffers();
void freeFramebuffers();

void initialiseDisplay();
void sleepDisplay();

void refreshScreen(uint16_t cycles);
void writeImage();

void fillBlackWhite();
void fillWhite();

void printHelp();
void printStatus();

void processCommand(char *command);
void processRefreshCommand();


// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(SERIAL_BAUD);

    delay(500);

    Serial.println();
    Serial.println("========================================");
    Serial.println("  2.66in BWR E-Ink Screen Utility");
    Serial.println("========================================");
    Serial.println();

    Serial.print("Initialising display... ");

    initialiseDisplay();

    Serial.println("OK");
    Serial.println();

    printHelp();
}


// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void loop()
{
    while (Serial.available() > 0)
    {
        char c = Serial.read();

        // Newline terminates a command
        if (c == '\n' || c == '\r')
        {
            if (serialBufferIndex > 0)
            {
                serialBuffer[serialBufferIndex] = '\0';

                processCommand(serialBuffer);

                serialBufferIndex = 0;
                memset(serialBuffer, 0, sizeof(serialBuffer));
            }
        }
        else
        {
            // Prevent buffer overflow
            if (serialBufferIndex < SERIAL_BUFFER_SIZE - 1)
            {
                serialBuffer[serialBufferIndex++] = c;
            }
        }
    }
}


// ===========================================================================
// Display initialisation
// ===========================================================================

void initialiseDisplay()
{
    DEV_Module_Init();

    EPD_2IN66BSES_Init();

    displayInitialised = true;
}


// ===========================================================================
// Allocate framebuffers
// ===========================================================================

bool allocateFramebuffers()
{
    if (BlackImage != NULL || RedImage != NULL)
    {
        freeFramebuffers();
    }

    ImageSize =
        ((EPD_2IN66BSES_WIDTH % 8 == 0)
            ? (EPD_2IN66BSES_WIDTH / 8)
            : (EPD_2IN66BSES_WIDTH / 8 + 1))
        * EPD_2IN66BSES_HEIGHT;


    Serial.print("Allocating framebuffer: ");
    Serial.print(ImageSize);
    Serial.println(" bytes each");


    BlackImage = (UBYTE *)malloc(ImageSize);

    if (BlackImage == NULL)
    {
        Serial.println("ERROR: Failed to allocate BlackImage");
        return false;
    }


    RedImage = (UBYTE *)malloc(ImageSize);

    if (RedImage == NULL)
    {
        Serial.println("ERROR: Failed to allocate RedImage");

        free(BlackImage);
        BlackImage = NULL;

        return false;
    }


    /*
     * Initialise both Paint images.
     *
     * The rotation is retained from the original Waveshare example.
     */

    Paint_NewImage(
        BlackImage,
        EPD_2IN66BSES_WIDTH,
        EPD_2IN66BSES_HEIGHT,
        270,
        WHITE
    );

    Paint_NewImage(
        RedImage,
        EPD_2IN66BSES_WIDTH,
        EPD_2IN66BSES_HEIGHT,
        270,
        WHITE
    );


    return true;
}


// ===========================================================================
// Free framebuffers
// ===========================================================================

void freeFramebuffers()
{
    if (BlackImage != NULL)
    {
        free(BlackImage);
        BlackImage = NULL;
    }

    if (RedImage != NULL)
    {
        free(RedImage);
        RedImage = NULL;
    }

    ImageSize = 0;
}


// ===========================================================================
// Full-screen BLACK / WHITE frame
// ===========================================================================

void fillBlackWhite()
{
    /*
     * Black channel:
     *   completely black
     *
     * Red channel:
     *   cleared to its inactive state.
     *
     * The original Waveshare example uses BLACK here for the
     * RedImage buffer, so retain that behaviour.
     */

    Paint_SelectImage(BlackImage);
    Paint_Clear(BLACK);

    Paint_SelectImage(RedImage);
    Paint_Clear(BLACK);
}


// ===========================================================================
// Full-screen WHITE frame
// ===========================================================================

void fillWhite()
{
    /*
     * Completely white display.
     */

    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    Paint_SelectImage(RedImage);
    Paint_Clear(BLACK);
}


// ===========================================================================
// Burn-in / ghosting cleaner
// ===========================================================================

void refreshScreen(uint16_t cycles)
{
    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("E-INK GHOSTING CLEANER");
    Serial.println("----------------------------------------");

    Serial.print("Cycles: ");
    Serial.println(cycles);

    Serial.print("Delay between refreshes: ");
    Serial.print(REFRESH_DELAY_MS);
    Serial.println(" ms");

    Serial.println();
    Serial.println("Starting...");
    Serial.println();


    // Allocate temporary display buffers
    if (!allocateFramebuffers())
    {
        Serial.println();
        Serial.println("Refresh aborted.");
        Serial.println();

        return;
    }


    for (uint16_t cycle = 0; cycle < cycles; cycle++)
    {
        Serial.print("Cycle ");
        Serial.print(cycle + 1);
        Serial.print("/");
        Serial.println(cycles);


        // ---------------------------------------------------------------
        // FULL BLACK
        // ---------------------------------------------------------------

        Serial.println("  -> BLACK");

        fillBlackWhite();

        EPD_2IN66BSES_Display(
            BlackImage,
            RedImage
        );

        DEV_Delay_ms(REFRESH_DELAY_MS);


        // ---------------------------------------------------------------
        // FULL WHITE
        // ---------------------------------------------------------------

        Serial.println("  -> WHITE");

        fillWhite();

        EPD_2IN66BSES_Display(
            BlackImage,
            RedImage
        );

        DEV_Delay_ms(REFRESH_DELAY_MS);
    }


    // -------------------------------------------------------------------
    // Finish on white
    // -------------------------------------------------------------------

    Serial.println();
    Serial.println("Final white refresh...");

    fillWhite();

    EPD_2IN66BSES_Display(
        BlackImage,
        RedImage
    );

    DEV_Delay_ms(REFRESH_DELAY_MS);


    // -------------------------------------------------------------------
    // Free framebuffer memory
    // -------------------------------------------------------------------

    freeFramebuffers();


    Serial.println();
    Serial.println("Ghosting cleaner complete.");
    Serial.println("Display left white.");
    Serial.println();


    // Put display into low-power state
    sleepDisplay();


    Serial.println("Ready.");
    Serial.println();
}


// ===========================================================================
// Write existing ImageData.c image
// ===========================================================================

void writeImage()
{
    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("WRITE IMAGE");
    Serial.println("----------------------------------------");


    if (!allocateFramebuffers())
    {
        Serial.println("Image display aborted.");
        return;
    }


    // -------------------------------------------------------------------
    // Black layer
    // -------------------------------------------------------------------

    Paint_SelectImage(BlackImage);

    Paint_Clear(WHITE);

    Paint_DrawBitMap(gImage_2in66bb);


    // -------------------------------------------------------------------
    // Red layer
    // -------------------------------------------------------------------

    Paint_SelectImage(RedImage);

    /*
     * This is intentionally BLACK to match the behaviour of the
     * original Waveshare example.
     */

    Paint_Clear(BLACK);


    // -------------------------------------------------------------------
    // Display
    // -------------------------------------------------------------------

    Serial.println("Writing image...");

    EPD_2IN66BSES_Display(
        BlackImage,
        RedImage
    );

    DEV_Delay_ms(2000);


    // -------------------------------------------------------------------
    // Cleanup
    // -------------------------------------------------------------------

    freeFramebuffers();

    Serial.println("Image written.");
    Serial.println();

    sleepDisplay();

    Serial.println("Ready.");
    Serial.println();
}


// ===========================================================================
// Display sleep
// ===========================================================================

void sleepDisplay()
{
    if (!displayInitialised)
    {
        return;
    }

    Serial.println("Putting display to sleep...");

    EPD_2IN66BSES_Sleep();

    Serial.println("Display sleeping.");
}


// ===========================================================================
// Help
// ===========================================================================

void printHelp()
{
    Serial.println("----------------------------------------");
    Serial.println("Available commands:");
    Serial.println();
    Serial.println("  refresh screen");
    Serial.println("      Run ghosting/burn-in cleaner.");
    Serial.println();
    Serial.println("  write image");
    Serial.println("      Display gImage_2in66bb.");
    Serial.println();
    Serial.println("  status");
    Serial.println("      Display current utility status.");
    Serial.println();
    Serial.println("  help");
    Serial.println("      Show this help.");
    Serial.println("----------------------------------------");
    Serial.println();
}


// ===========================================================================
// Status
// ===========================================================================

void printStatus()
{
    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("STATUS");
    Serial.println("----------------------------------------");

    Serial.print("Display initialised: ");

    if (displayInitialised)
    {
        Serial.println("YES");
    }
    else
    {
        Serial.println("NO");
    }


    Serial.print("Display size: ");
    Serial.print(EPD_2IN66BSES_WIDTH);
    Serial.print(" x ");
    Serial.println(EPD_2IN66BSES_HEIGHT);


    Serial.print("Framebuffer size: ");

    if (ImageSize > 0)
    {
        Serial.print(ImageSize);
        Serial.println(" bytes");
    }
    else
    {
        Serial.println("not allocated");
    }


    Serial.print("Black framebuffer: ");

    if (BlackImage != NULL)
    {
        Serial.println("allocated");
    }
    else
    {
        Serial.println("free");
    }


    Serial.print("Red framebuffer: ");

    if (RedImage != NULL)
    {
        Serial.println("allocated");
    }
    else
    {
        Serial.println("free");
    }

    Serial.println("----------------------------------------");
    Serial.println();
}


// ===========================================================================
// Refresh command
// ===========================================================================

void processRefreshCommand()
{
    Serial.println();
    Serial.println("Select refresh count:");
    Serial.println();
    Serial.println("  10");
    Serial.println("  25");
    Serial.println("  50");
    Serial.println("  100");
    Serial.println();

    Serial.print("Cycles: ");


    // Wait for cycle selection
    unsigned long startTime = millis();

    char choiceBuffer[16];
    uint8_t choiceIndex = 0;

    memset(choiceBuffer, 0, sizeof(choiceBuffer));


    while (millis() - startTime < 30000)
    {
        while (Serial.available() > 0)
        {
            char c = Serial.read();


            if (c == '\n' || c == '\r')
            {
                if (choiceIndex == 0)
                {
                    continue;
                }

                choiceBuffer[choiceIndex] = '\0';


                int cycles = atoi(choiceBuffer);


                if (
                    cycles == 10 ||
                    cycles == 25 ||
                    cycles == 50 ||
                    cycles == 100
                )
                {
                    refreshScreen((uint16_t)cycles);
                    return;
                }


                Serial.println();
                Serial.println("Invalid selection.");
                Serial.println("Please enter 10, 25, 50, or 100.");
                Serial.print("Cycles: ");

                choiceIndex = 0;
                memset(choiceBuffer, 0, sizeof(choiceBuffer));

                continue;
            }


            if (choiceIndex < sizeof(choiceBuffer) - 1)
            {
                choiceBuffer[choiceIndex++] = c;
                Serial.write(c);
            }
        }
    }


    Serial.println();
    Serial.println("Refresh selection timed out.");
    Serial.println();
}


// ===========================================================================
// Command processor
// ===========================================================================

void processCommand(char *command)
{
    // Remove leading whitespace

    while (*command == ' ' || *command == '\t')
    {
        command++;
    }


    // Convert command to lowercase

    for (char *p = command; *p != '\0'; p++)
    {
        if (*p >= 'A' && *p <= 'Z')
        {
            *p = *p + ('a' - 'A');
        }
    }


    // -----------------------------------------------------------------------
    // HELP
    // -----------------------------------------------------------------------

    if (strcmp(command, "help") == 0)
    {
        printHelp();
        return;
    }


    // -----------------------------------------------------------------------
    // STATUS
    // -----------------------------------------------------------------------

    if (strcmp(command, "status") == 0)
    {
        printStatus();
        return;
    }


    // -----------------------------------------------------------------------
    // REFRESH SCREEN
    // -----------------------------------------------------------------------

    if (strcmp(command, "refresh screen") == 0)
    {
        processRefreshCommand();
        return;
    }


    // -----------------------------------------------------------------------
    // WRITE IMAGE
    // -----------------------------------------------------------------------

    if (strcmp(command, "write image") == 0)
    {
        writeImage();
        return;
    }


    // -----------------------------------------------------------------------
    // UNKNOWN COMMAND
    // -----------------------------------------------------------------------

    Serial.print("Unknown command: ");
    Serial.println(command);

    Serial.println("Type 'help' for available commands.");
    Serial.println();
}
