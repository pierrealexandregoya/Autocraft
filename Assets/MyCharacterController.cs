using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MyCharacterController : MonoBehaviour
{
    //private Vector3 direction = new Vector3(0, 0, 1);
    private Rigidbody rb;
    public float m_speed = 5f;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    private void Update()
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
}
