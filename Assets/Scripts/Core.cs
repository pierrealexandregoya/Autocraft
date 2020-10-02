using System.Collections.Generic;
using UnityEngine;
using System;
using System.Collections.Concurrent;
using UnityEngine.UI;
using System.Threading;
using System.Threading.Tasks;
using NativeWebSocket;

namespace AutoCraft
{

    public class Core : MonoBehaviour
    {
        public enum State
        {
            None,
            Connecting,
            Authenticating,
            Starting,
            Playing,
        }

        public static ConcurrentQueue<Update> updates = new ConcurrentQueue<Update>();
        public static Dictionary<int, GameObject> objects = new Dictionary<int, GameObject>();

        public static Mutex globalMutex;
        public static Text statusText;
        public static string statusStr;
        public static Vector3 spawnPosition;

        public static State state = State.None;

        private static WebSocket webSocket;
        //private static Task connectTask;
        //private static CancellationToken ct;

        public static bool Started()
        {
            return state >= State.Playing;
        }

        public static void SetStatus(string newStatusStr)
        {
            globalMutex.WaitOne();
            statusStr = newStatusStr;
            globalMutex.ReleaseMutex();
        }

        public static void PushUpdate(Update update)
        {
            globalMutex.WaitOne();
            updates.Enqueue(update);
            globalMutex.ReleaseMutex();
        }

        private void SetActiveAllChildren(Transform transform, bool value)
        {
            foreach (Transform child in transform)
            {
                child.gameObject.SetActive(value);
                SetActiveAllChildren(child, value);
                //Debug.Log($"{child.gameObject.name}");
            }
        }

        void Start()
        {
            var characterController = GameObject.Find("CharacterController");
            var mainCamera = GameObject.Find("MainCamera");
            var mainCanvas = GameObject.Find("MainCanvas");
            var status = GameObject.Find("Status");
            SetActiveAllChildren(transform, false);
            characterController.SetActive(true);
            mainCamera.SetActive(true);
            mainCanvas.SetActive(true);
            status.SetActive(true);


            globalMutex = new Mutex();

            GameObject canvas = GameObject.FindGameObjectWithTag("Canvas");
            if (!canvas)
            {
                Debug.LogError("Could not find canvas");
                return;
            }

            GameObject statusGO = canvas.transform.Find("Status").gameObject;
            if (!statusGO)
            {
                Debug.LogError("Could not find status text GameObject");
                return;
            }

            statusText = statusGO.GetComponent<Text>();
            if (!statusText)
            {
                Debug.LogError("Could not find status text component");
                return;
            }

            webSocket = new WebSocket("ws://localhost:12000");

            statusText.text = "Connecting...";
            webSocket.OnOpen += () =>
            {
                AuthRequest authRequest = new AuthRequest();
                authRequest.playerName = "Player123";
                webSocket.SendText(JsonUtility.ToJson(authRequest));
                statusText.text = $"Authenticating...";
                state = State.Authenticating;
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
                        statusText.text = "Server refused authentication";
                        return;
                    }
                    spawnPosition = authRecipe.spawnPosition;
                    statusText.text = "";
                    state = State.Starting;
                }
            };


            state = State.Connecting;

            _ = webSocket.Connect();
        }

        void Update()
        {
#if !UNITY_WEBGL || UNITY_EDITOR
            webSocket.DispatchMessageQueue();
#endif
            if (state == State.Starting)
            {
                SetActiveAllChildren(transform, true);
                GameObject.Find("CharacterController").transform.position = spawnPosition;
                state = State.Playing;
            }
            if (state < State.Playing)
                return;
            //globalMutex.WaitOne();
            //statusText.text = statusStr;
            //globalMutex.ReleaseMutex();

            //    if (updates.Count > 0)
            //    {
            //        GameObject prefab = Resources.Load<GameObject>("Prefabs/Cube");
            //        Update update;
            //        while (updates.TryDequeue(out update))
            //        {
            //            if (update.type == "create")
            //            {
            //                GameObject newGO = Instantiate(prefab, update.vector, new UnityEngine.Quaternion());
            //                objects[update.id] = newGO;
            //            }
            //            else if (update.type == "addForce")
            //            {
            //                GameObject go;
            //                if (!objects.TryGetValue(update.id, out go))
            //                    Debug.LogError($"Client.Update: addForce: id \"{update.id}\" not found");
            //                else
            //                    go.GetComponent<Rigidbody>().AddForce(update.vector);
            //            }
            //        }
            //    }
        }

        void OnDestroy()
        {
            _ = webSocket.Close();
            //AsynchronousClient.socket.Shutdown(SocketShutdown.Both);
            //AsynchronousClient.socket.Close();
        }
    }
}