#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <numeric>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#endif

// ===== Serial =====

#ifdef _WIN32
HANDLE open_serial(const char* port)
{
    HANDLE h = CreateFileA(port,
        GENERIC_READ,
        0, NULL, OPEN_EXISTING, 0, NULL);

    return h;
}

std::string read_serial(HANDLE h)
{
    char buf[64];
    DWORD read;
    ReadFile(h, buf, sizeof(buf), &read, NULL);
    return std::string(buf, read);
}

#else

int open_serial(const char* port)
{
    int fd = open(port, O_RDONLY);

    termios tty{};
    tcgetattr(fd, &tty);

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}

std::string read_serial(int fd)
{
    char buf[64];
    int n = read(fd, buf, sizeof(buf));
    return std::string(buf, n);
}

#endif

// ===== Logs =====

void append(const std::string& file, const std::string& s)
{
    std::ofstream f(file, std::ios::app);
    f << s << std::endl;
}

// ===== Main =====

int main()
{
#ifdef _WIN32
    auto serial = open_serial("\\\\.\\COM3");
#else
    auto serial = open_serial("/dev/ttyUSB0");
#endif

    std::vector<double> hourTemps;
    std::vector<double> dayTemps;

    auto hourStart = std::chrono::steady_clock::now();
    auto dayStart = std::chrono::steady_clock::now();

    while (true)
    {
        auto data = read_serial(serial);

        if (data.size())
        {
            double t = atof(data.c_str());

            append("all.log", data);

            hourTemps.push_back(t);
            dayTemps.push_back(t);
        }

        auto now = std::chrono::steady_clock::now();

        // Hour avg
        if (std::chrono::duration_cast<std::chrono::hours>(now - hourStart).count() >= 1)
        {
            double avg = std::accumulate(hourTemps.begin(), hourTemps.end(), 0.0) / hourTemps.size();
            append("hour.log", std::to_string(avg));

            hourTemps.clear();
            hourStart = now;
        }

        // Day avg
        if (std::chrono::duration_cast<std::chrono::hours>(now - dayStart).count() >= 24)
        {
            double avg = std::accumulate(dayTemps.begin(), dayTemps.end(), 0.0) / dayTemps.size();
            append("day.log", std::to_string(avg));

            dayTemps.clear();
            dayStart = now;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
