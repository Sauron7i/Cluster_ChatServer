#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User
{
public:
    User(int id = -1, string name = "", string password = "", string state = "offline")
        : m_id(id), m_name(name), m_password(password), m_state(state)
    {}

    void setId(int id) { this->m_id = id; }
    void setName(string name) { this->m_name = name; }
    void setPwd(string password) { this->m_password = password; }
    void setState(string state) { this->m_state = state; }

    int getId() const { return this->m_id; }
    string getName() const { return this->m_name; }
    string getPwd() const { return this->m_password; }
    string getState() const { return this->m_state; }

protected:
    int     m_id;
    string  m_name;
    string  m_password;
    string  m_state;
};

#endif