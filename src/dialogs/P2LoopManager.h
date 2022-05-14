#pragma once

#include <steam_api.h>

struct SteamHelper {
    SteamHelper() = delete;

    //Boolean variable that dictates the loop state, true cuts the loop while false keeps the loop alive when called.
    inline static bool g_steamAPICallbacksSetLoaded = true;
    

    //Sets the API state variable to false as well as call the while loop, locking the thread until FinishLoopCall() is called.
    inline static void StartLoopCall()
    {
        if(g_steamAPICallbacksSetLoaded){
            g_steamAPICallbacksSetLoaded = false; 
            BaseSteamCallbackLoop();
        }
    }
    //This initiates the API state variable and a while loop function, Steam's SteamAPI_RunCallbacks relies on it being called when needed, usually in games this can be called every render frame, however, This application is single threadded, meaning if we constantly call it every "tick" we'll end up locking up the application. So we only call it when it's needed, and cancel out of the loop when we have the data we need. this also allows us to lock the application when it's getting data, preventing the user from messing stuff up while it's busy processing the data.  
    inline static void BaseSteamCallbackLoop()
    { 
        while (!g_steamAPICallbacksSetLoaded) 
        {
            SteamAPI_RunCallbacks();
        }
    }

    //Sets the API state variable to true so the while loop cuts, this is usually put at the end of a API call result function.
    inline static void FinishLoopCall() 
    {
        g_steamAPICallbacksSetLoaded = true;
    }

};
