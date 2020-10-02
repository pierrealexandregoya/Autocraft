using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using UnityEngine;

namespace AutoCraft
{
    public class PlayerItems : MonoBehaviour
    {
        public GameObject itemSlotPrefab;
        private GameObject weaponInstance;
        private const UInt16 nbItemSlots = 6;
        private UInt16 selectedSlotIdx = 0;

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public abstract class Item
        {
            public UInt32 Id { get; set; }
            public string Name { get; set; }
            public string IconName { get; set; }
            public string ModelName { get; set; }
            public UnityEngine.Vector3 Rotation { get; set; }
            public abstract void Update(GameObject gameObject);
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public abstract class Weapon : Item
        {
            public GameObject RayInstance { get; set; }
            public bool Casting { get; set; } = false;
            public abstract override void Update(GameObject gameObject);
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public class ItemNone : Item
        {
            public ItemNone()
            {
                Id = 0;
                Name = "None";
            }
            public override void Update(GameObject gameObject) { }
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public class LaserRifle : Weapon
        {
            public LaserRifle()
            {
                Id = 1;
                Name = "Laser Rifle";
                IconName = "LaserRifle";
                ModelName = "LaserRifle";
                Rotation = new UnityEngine.Vector3(0, 180, 0);
            }

            public override void Update(GameObject gameObject)
            {

            }
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public class PushPullGun : Weapon
        {
            public PushPullGun()
            {
                Id = 2;
                Name = "Push Pull Gun";
                IconName = "PushPullGun";
                ModelName = "PushPullGun";
                Rotation = new UnityEngine.Vector3(-90, -90, 0);
            }

            public override void Update(GameObject weaponInstance)
            {
                if ((Input.GetKeyDown(KeyCode.Mouse0) || Input.GetKeyDown(KeyCode.Mouse1)) && !Casting)
                {
                    GameObject firePoint = weaponInstance.transform.Find("FirePoint").gameObject;
                    if (!firePoint)
                    {
                        Debug.LogError("FirePoint gameobject not found for Laser Rifle weapon");
                        return;
                    }

                    string prefabName;
                    if (Input.GetKeyDown(KeyCode.Mouse0))
                        prefabName = "RedRay";
                    else
                        prefabName = "BlueRay";

                    GameObject rayPrefab = Resources.Load<GameObject>("Prefabs/" + prefabName);
                    if (!rayPrefab)
                    {
                        Debug.LogError("Did not find required ray game object for Laser Rifle weapon");
                        return;
                    }

                    Casting = true;
                    RayInstance = Instantiate(rayPrefab, firePoint.transform);
                    RayInstance.transform.Rotate(0, 0, 90);
                    RayInstance.transform.Translate(new UnityEngine.Vector3(-7.5f * rayPrefab.transform.localScale.x, 0, 0), firePoint.transform);

                    GameObject empty = new GameObject();
                    GameObject target = Instantiate(empty, firePoint.transform);
                    target.transform.Translate(new UnityEngine.Vector3(100, 0, 0));

                    RaycastHit hitInfo;
                    if (Physics.Raycast(firePoint.transform.position, target.transform.position, out hitInfo, 100))
                    {
                        Rigidbody targetRB = hitInfo.collider.attachedRigidbody;
                        UnityEngine.Vector3 force = (targetRB.gameObject.transform.position - firePoint.transform.position).normalized * 50;
                        if (Input.GetKeyDown(KeyCode.Mouse1))
                            force = -force;
                        targetRB.AddForce(force);
                    }
                }
                else if ((Input.GetKeyUp(KeyCode.Mouse0) || Input.GetKeyUp(KeyCode.Mouse1)) && Casting)
                {
                    Destroy(RayInstance);
                    Casting = false;
                }
            }
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public class PlayerItem
        {
            public UInt16 Slot { get; set; }
            public Item Item { get; set; }
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        List<PlayerItem> PlayerItemsInBar = new List<PlayerItem>()
    {
        new PlayerItem{Slot = 0, Item = new PushPullGun()},
        new PlayerItem{Slot = 1, Item = new LaserRifle()},
    };

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        public PlayerItem GetPlayerItem(UInt16 idx)
        {
            foreach (PlayerItem playerItem in PlayerItemsInBar)
            {
                if (playerItem.Slot == idx)
                    return playerItem;
            }
            return new PlayerItem { Slot = 42, Item = new ItemNone() };
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

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

            string charCtrlName = "CharacterController";
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

            Item weaponItem = GetPlayerItem(slotIdx).Item;
            if (weaponItem.Name != "None")
            {
                string modelPath = "Prefabs/" + weaponItem.ModelName;
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

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        void Start()
        {
            Cursor.visible = false;
            if (!itemSlotPrefab)
                Debug.LogError("ItemBar: no prefab");

            GameObject anchor = this.gameObject.transform.Find("Anchor").gameObject;
            if (!anchor)
                Debug.LogError("No anchor found");


            int n = 0;
            foreach (PlayerItem playerItem in PlayerItemsInBar)
            {
                GameObject go = Instantiate(itemSlotPrefab, anchor.transform.position + new UnityEngine.Vector3(n, 0, 0), new UnityEngine.Quaternion(), anchor.transform);
                GameObject imageGo = go.transform.Find("Image").gameObject;
                if (!imageGo)
                    Debug.LogError("No \"Image\" child found");

                UnityEngine.UI.Image image = imageGo.GetComponent<UnityEngine.UI.Image>();
                string spritePath = "Images/" + playerItem.Item.IconName;
                Sprite sprite = Resources.Load<Sprite>(spritePath);
                if (!sprite)
                    Debug.LogError($"sprite \"{spritePath}\" not found");

                image.sprite = sprite;

                n += 62;
            }

            SelectItemSlot(0);
        }

        /*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

        void Update()
        {
            if (!Core.Started())
                return;

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

            Item itemToUpdate = GetPlayerItem(selectedSlotIdx).Item;
            if (itemToUpdate.Name != "None")
                itemToUpdate.Update(weaponInstance);
        }
    }

}