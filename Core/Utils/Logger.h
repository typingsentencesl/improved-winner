namespace Log
{
    const HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    void Info(const std::string& message)
    {
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY);
        std::cout << oxorany("[INFO] ") << message << std::endl;
    }

    void LogWarning(const std::string& message, bool waitForKey = false)
    {
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << oxorany("[WARN] ") << message << std::endl;

        if (waitForKey)
        {
            SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY);
            // system("pause");
        }
    }

    void Error(const std::string& message, bool terminate = true)
    {
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << oxorany("[ERROR] ") << message << std::endl;
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY);
        system(oxorany("pause"));

        if (terminate)
        {
            exit(EXIT_FAILURE);
        }
    }

    void LogSuccess(const std::string& message)
    {
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << oxorany("[SUCCESS] ") << message << std::endl;
    }

    void LogDebug(const std::string& message, bool saveToFile = false)
    {
#ifdef DEBUG_MODE
        std::string debugMessage = oxorany("[DEBUG] ") + message + '\n';

        SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        std::cout << debugMessage;

        if (saveToFile)
        {
            // SaveLog(debugMessage);
        }
#endif
    }

    void LogCustom(const std::string& message, int textColor)
    {
        SetConsoleTextAttribute(consoleHandle, textColor);
        std::cout << message << std::endl;
    }

    void ClearPreviousLine()
    {
        std::cout << oxorany("\033[1A\033[2K\r");
    }
}