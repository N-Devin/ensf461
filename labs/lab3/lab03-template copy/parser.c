#include "parser.h"
#include <ctype.h>
// Command to trim whitespace and ASCII control characters from buffer
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t trimstring(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    // Copy the input buffer to the output buffer
    strncpy(outputbuffer, inputbuffer, bufferlen);
    outputbuffer[bufferlen - 1] = '\0'; // Null-terminate the output buffer

    // Find the last non-whitespace, non-control character
    size_t len = strlen(outputbuffer);
    for (size_t ii = len; ii > 0; ii--)
    {
        if (!isspace((unsigned char)outputbuffer[ii - 1]) && outputbuffer[ii - 1] >= ' ')
        {
            break;
        }
        outputbuffer[ii - 1] = '\0'; // Null-terminate the trimmed string
    }

    return strlen(outputbuffer);
}

// Command to trim the input command to just be the first word
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t firstword(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    // Initialize the output buffer
    memset(outputbuffer, 0, bufferlen);

    // Find the first space or delimiter
    const char *delimiter = " ";
    const char *first_space = strchr(inputbuffer, delimiter[0]);

    if (first_space != NULL)
    {
        // Calculate the length of the first word
        size_t word_length = first_space - inputbuffer;

        // Ensure the output buffer has enough space for the first word
        if (word_length < bufferlen)
        {
            // Copy the first word to the output buffer
            strncpy(outputbuffer, inputbuffer, word_length);
        }
    }
    else
    {
        // No space or delimiter found, so just copy the entire input
        strncpy(outputbuffer, inputbuffer, bufferlen - 1);
    }

    return strlen(outputbuffer);
}

// Command to test that string only contains valid ascii characters (non-control and not extended)
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if no invalid ASCII characters present
bool isvalidascii(const char *inputbuffer, size_t bufferlen)
{
    size_t stringlength = strlen(inputbuffer);

    bool isValid = true;

    for (size_t ii = 0; ii < stringlength; ii++)
    {
        isValid &= (inputbuffer[ii] >= ' ' && inputbuffer[ii] <= '~');
        // Check if the character is within the printable ASCII range
    }

    return isValid;
}

// Command to find location of pipe character in input string
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] int - location in the string of the pipe character, or -1 pipe character not found
int findpipe(const char *inputbuffer, size_t bufferlen)
{
    // Iterate through the characters in the input buffer
    for (size_t i = 0; i < bufferlen; i++)
    {
        if (inputbuffer[i] == '|')
        {
            // Return the index of the pipe character when found
            return (int)i;
        }
    }

    // Return -1 if the pipe character was not found
    return -1;
}

// Command to test whether the input string ends with "&" and
// thus represents a command that should be run in background
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if string ends with "&"
bool runinbackground(const char *inputbuffer, size_t bufferlen)
{
    // Ensure bufferlen is greater than 0 to avoid buffer underflow
    if (bufferlen == 0)
    {
        return false;
    }

    // Calculate the index of the last character in the input buffer
    size_t lastIndex = bufferlen - 1;

    // Check if the last character is an ampersand ('&')
    if (inputbuffer[lastIndex] == '&')
    {
        return true; // Return true to indicate running in the background
    }

    // Return false if the input does not end with an ampersand
    return false;
}
