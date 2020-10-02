using NativeWebSocket;
using System.Threading;
using UnityEngine;

namespace AutoCraft
{
    public class Server
    {

        private enum State
        {
            Connecting,
            Authenticating,
            Starting,
            Playing,
        }

        private static WebSocket webSocket;
        private static State state = State.Connecting;

        public static void Init()
        {
            webSocket = new WebSocket("ws://localhost:12000");

            webSocket.OnOpen += () =>
            {
                Debug.Log("Connected!");
                AuthRequest authRequest = new AuthRequest();
                authRequest.playerName = "_master";
                webSocket.SendText(JsonUtility.ToJson(authRequest));
            };

            webSocket.OnError += (e) =>
            {
                Debug.Log("Error! " + e);
            };

            webSocket.OnClose += (e) =>
            {
                Debug.Log("Connection closed!");
            };

            webSocket.OnMessage += (bytes) =>
            {
                var message = System.Text.Encoding.UTF8.GetString(bytes);
                Debug.Log("OnMessage! " + message);
                if (state == State.Authenticating)
                {
                    AuthRecipe authRecipe = JsonUtility.FromJson<AuthRecipe>(message);
                    if (!authRecipe.authenticated)
                    {
                        return;
                    }
                    state = State.Playing;
                }
            };


            state = State.Connecting;

            _ = webSocket.Connect();

            Thread t = new Thread(() =>
            {
                Loop();
            });
            t.Start();
            t.Join();
        }

        private static void Loop()
        {
            while (true)
            {
                Thread.Sleep(25);
                webSocket.DispatchMessageQueue();
            }
        }

    }

}