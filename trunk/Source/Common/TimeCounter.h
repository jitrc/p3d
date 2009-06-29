#pragma once

namespace P3D
{
    /*
    High precision time counter.
    */
    class TimeCounter
    {
        static uint64 GetTickCount();
    public:
        TimeCounter();

        /*
        Reset time counting.
        */
        void Reset()
        {
            _lastTickTime = 0;
            _startTick = GetTickCount();
            _lastTick = _startTick;
            _ticks = 0;
        }

        float Tick()
        {
            uint64 tick = GetTickCount();
            _lastTickTime=float(((tick - _lastTick)*1000.0f)/_ticksPerSec);
            _lastTick=tick;
            _ticks++;
            return _lastTickTime;
        }

        double GetTotalTime() const { return double(((_lastTick - _startTick)*1000.0f)/_ticksPerSec); }
        float GetLastTick() const { return _lastTickTime; }

        /*
        Return count of calls to Tick since call to Reset.
        */
        uint GetTicks() const { return _ticks; }

    protected:
        float _lastTickTime;

        uint _ticks;
        uint64 _startTick;
        uint64 _lastTick;
        uint64 _ticksPerSec;
    };
}

