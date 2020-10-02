using System;

namespace AutoCraft
{
    [Serializable]
    public class AuthRequest
    {
        public string playerName;
    }

    [Serializable]
    public class AuthRecipe
    {
        public bool authenticated;
        public UnityEngine.Vector3 spawnPosition;
    }

    [Serializable]
    public class Update
    {
        public string type;
        public int id;
        public UnityEngine.Vector3 vector;
    }
}
