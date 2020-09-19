using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class PlayerItems : MonoBehaviour
{
    public GameObject itemSlotPrefab;
    private GameObject weaponInstance;
    private const UInt16 nbItemSlots = 6;
    private UInt16 selectedSlotIdx = 0;
    private float scrollMouseDelta = 0;
    public class Item
    {
        public UInt32 id { get; set; }
        public string name { get; set; }
        public string iconName { get; set; }
        public string modelName { get; set; }
        public UnityEngine.Vector3 rotation { get; set; }
    }

    List<Item> items = new List<Item>()
    {
        new Item{id = 0, name = "None", iconName = "", modelName = "", rotation = new UnityEngine.Vector3(0, 0, 0)},
        new Item{id = 1, name = "Laser Rifle", iconName = "LaserRifle", modelName = "LaserRifle", rotation = new UnityEngine.Vector3(0, 180, 0)},
        new Item{id = 2, name = "Push Pull Gun", iconName = "PushPullGun", modelName = "PushPullGun", rotation = new UnityEngine.Vector3(-90, -90, 0)},
    };

    public class PlayerItem
    {
        public UInt16 slot;
        public UInt32 itemId;
    }

    List<PlayerItem> playerItems = new List<PlayerItem>()
    {
        new PlayerItem{slot = 0, itemId = 2},
        new PlayerItem{slot = 1, itemId = 1},
    };

    public Item GetItem(UInt32 id)
    {
        foreach(Item item in items)
        {
            if (item.id == id)
                return item;
        }
        return items.ElementAt(0);
    }

    public PlayerItem GetPlayerItem(UInt16 idx)
    {
        foreach (PlayerItem playerItem in playerItems)
        {
            if (playerItem.slot == idx)
                return playerItem;
        }
        return new PlayerItem { itemId = 0 };
    }

    void Start()
    {
        if (!itemSlotPrefab)
            Debug.LogError("ItemBar: no prefab");

        //Debug.Log(this.gameObject.name);
        GameObject anchor = this.gameObject.transform.Find("Anchor").gameObject;
        if (!anchor)
            Debug.LogError("No anchor found");

        int n = 0;
        foreach (PlayerItem playerItem in playerItems)
        {
            GameObject go = Instantiate(itemSlotPrefab, anchor.transform.position + new UnityEngine.Vector3(n, 0, 0), new UnityEngine.Quaternion(), anchor.transform);
            GameObject imageGo = go.transform.Find("Image").gameObject;
            if (!imageGo)
                Debug.LogError("No \"Image\" child found");

            UnityEngine.UI.Image image = imageGo.GetComponent<UnityEngine.UI.Image>();
            string spritePath = "Images/" + GetItem(playerItem.itemId).iconName;
            Sprite sprite = Resources.Load<Sprite>(spritePath);
            if (!sprite)
                Debug.LogError($"sprite \"{spritePath}\" not found");
            
            image.sprite = sprite;

            n = n + 62;
        }

        SelectItemSlot(0);
    }

    void SelectItemSlot(UInt16 slotIdx)
    {
        GameObject anchor = this.gameObject.transform.Find("Anchor").gameObject;
        if (!anchor)
        {
            Debug.LogError("ItemBar: could not find \"Anchor\" child");
            return;
        }

        GameObject selectSquare = anchor.transform.Find("SelectSquare").gameObject;
        if (!selectSquare)
        {
            Debug.LogError("ItemBar: Anchor: could not find \"SelectSquare\" child");
            return;
        }

        string charCtrlName = "MyCharacterController";
        GameObject charCtrl = GameObject.Find(charCtrlName);

        if (!charCtrl)
        {
            Debug.LogError($"Character Controller named \"{charCtrlName}\" not found");
            return;
        }

        string mainCamName = "MainCamera";
        GameObject mainCam = charCtrl.transform.Find(mainCamName).gameObject;

        if (!mainCam)
        {
            Debug.LogError($"Camera named \"{mainCamName}\" not found");
            return;
        }

        string weaponAnchorName = "WeaponAnchor";
        GameObject weaponAnchor = mainCam.transform.Find(weaponAnchorName).gameObject;

        if (!weaponAnchor)
        {
            Debug.LogError($"Weapon Anchor named \"{weaponAnchorName}\" not found");
            return;
        }

        if (weaponInstance)
            Destroy(weaponInstance);

        Item weaponItem = GetItem(GetPlayerItem(slotIdx).itemId);
        if (weaponItem.name != "None")
        {
            string modelPath = "Prefabs/" + weaponItem.modelName;
            GameObject weaponModel = Resources.Load<GameObject>(modelPath);
            if (!weaponModel)
            {
                Debug.LogError($"Weapon at path \"{modelPath}\" not found");
                return;
            }

            weaponInstance = Instantiate(weaponModel, weaponAnchor.transform, false);
        }
        selectSquare.transform.position = anchor.transform.position + new UnityEngine.Vector3(slotIdx * 62, 0, 0);

        selectedSlotIdx = slotIdx;
    }

    void Update()
    {
        for (UInt16 i = 1; i <= nbItemSlots; ++i)
        {
            if (Input.GetKeyDown(i.ToString()))
            {
                SelectItemSlot((UInt16)(i - 1));
                break;
            }    
        }

        if (Input.mouseScrollDelta.y > 0 && selectedSlotIdx < nbItemSlots - 1)
            SelectItemSlot((UInt16)(selectedSlotIdx + 1));
        else if (Input.mouseScrollDelta.y < 0 && selectedSlotIdx > 0)
            SelectItemSlot((UInt16)(selectedSlotIdx - 1));
    }
}
