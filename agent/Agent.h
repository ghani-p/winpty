#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QPoint>
#include <windows.h>
#include "EventLoop.h"

class Win32Console;
class QLocalSocket;
class Terminal;
class QTimer;
class ReadBuffer;
class NamedPipe;

const int BUFFER_LINE_COUNT = 3000; // TODO: Use something like 9000.
const int MAX_CONSOLE_WIDTH = 500;

class Agent : public EventLoop
{
public:
    Agent(LPCWSTR controlPipeName,
          LPCWSTR dataPipeName,
          int initialCols,
          int initialRows);
    ~Agent();

private:
    NamedPipe *makeSocket(LPCWSTR pipeName);
    void resetConsoleTracking(bool sendClear = true);

private slots:
    void controlSocketReadyRead();
    void handlePacket(ReadBuffer &packet);
    int handleStartProcessPacket(ReadBuffer &packet);
    int handleSetSizePacket(ReadBuffer &packet);
    void dataSocketReadyRead();
    void socketDisconnected();

protected:
    virtual void onPollTimeout();
    virtual void onPipeIo();

private:
    void markEntireWindowDirty();
    void scanForDirtyLines();
    void resizeWindow(int cols, int rows);
    void scrapeOutput();
    void freezeConsole();
    void unfreezeConsole();
    void syncMarkerText(CHAR_INFO *output);
    int findSyncMarker();
    void createSyncMarker(int row);

private:
    Win32Console *m_console;
    NamedPipe *m_controlSocket;
    NamedPipe *m_dataSocket;
    Terminal *m_terminal;
    QTimer *m_timer;
    HANDLE m_childProcess;
    int m_childExitCode;

    int m_syncRow;
    int m_syncCounter;

    int m_scrapedLineCount;
    int m_scrolledCount;
    int m_maxBufferedLine;
    CHAR_INFO (*m_bufferData)[MAX_CONSOLE_WIDTH];
    int m_dirtyWindowTop;
    int m_dirtyLineCount;
};

#endif // AGENT_H
