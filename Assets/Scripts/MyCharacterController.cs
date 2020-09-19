using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MyCharacterController : MonoBehaviour
{
    public float m_speed = 5f;
    public enum RotationAxes { MouseXAndY = 0, MouseX = 1, MouseY = 2 }
    public RotationAxes axes = RotationAxes.MouseXAndY;
    public float sensitivityX = 15F;
    public float sensitivityY = 15F;
    public float minimumX = -360F;
    public float maximumX = 360F;
    public float minimumY = -60F;
    public float maximumY = 60F;
    float rotationY = 0F;

    void Start()
    {

    }

    private void Update()
    {
        Keyboard();
        if (!Input.GetKey(KeyCode.LeftAlt))
            Mouse();
    }

    private void Keyboard()
    {
        float speed = m_speed;

        Vector3 moveVector = Vector3.zero;
        if (Input.GetKey(KeyCode.A))
            moveVector += Vector3.Cross(transform.forward, transform.up);
        if (Input.GetKey(KeyCode.D))
            moveVector += -Vector3.Cross(transform.forward, transform.up);

        if (Input.GetKey(KeyCode.W))
            moveVector += transform.forward;
        if (Input.GetKey(KeyCode.S))
            moveVector += -transform.forward;

        if (Input.GetKey(KeyCode.Q))
            moveVector += -transform.up;
        if (Input.GetKey(KeyCode.E))
            moveVector += transform.up;

        if (Input.GetKey(KeyCode.LeftShift))
            speed = m_speed * 1.7f;

        moveVector.Normalize();

        if (moveVector != Vector3.zero)
            transform.position += moveVector * speed * Time.deltaTime;
    }

    private void Mouse()
    {
        if (axes == RotationAxes.MouseXAndY)
        {
            float rotationX = transform.localEulerAngles.y + Input.GetAxis("Mouse X") * sensitivityX;

            rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
            rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

            transform.localEulerAngles = new Vector3(-rotationY, rotationX, 0);
        }
        else if (axes == RotationAxes.MouseX)
        {
            transform.Rotate(0, Input.GetAxis("Mouse X") * sensitivityX, 0);
        }
        else
        {
            rotationY += Input.GetAxis("Mouse Y") * sensitivityY;
            rotationY = Mathf.Clamp(rotationY, minimumY, maximumY);

            transform.localEulerAngles = new Vector3(-rotationY, transform.localEulerAngles.y, 0);
        }
    }
}
