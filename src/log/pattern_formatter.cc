#include <walle/log/pattern_formatter.h>
#include <walle/log/message.h>
#include <walle/config/base.h>
#include <walle/time/date_time.h>
#include <walle/sys/env.h>
#include <walle/time/time_zone.h>
#include <walle/format/time_formatter.h>
#include <string>
#include <iostream>

namespace walle {
namespace log {

class flag_formatter {
public:
    virtual ~flag_formatter()
    {

    }
    virtual void format(Message &msg, walle::DateTime &tm) = 0;
};

class V_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<msg.log_name;
    }
};
 
class s_formatter : public flag_formatter {
public:
    s_formatter() 
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('s'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class v_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &dt)
    {
        msg.formatted<<walle::StringRef(msg.raw.data(), msg.raw.size());
    }
};

class l_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<static_cast<int>(msg.level);
    }
};

class p_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<level_str(msg.level);
    }
};

class q_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<level_short_str(msg.level);
    }
};

class P_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<msg.pid;
    }
};

class T_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<msg.thread_name;
    }
};

class I_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<msg.tid;
    }
};

static walle::StringRef cache_node_name()
{
    static std::string node_name;
    if(WALLE_UNLIKELY(node_name.empty())) {
        node_name = Environment::node_name(); 
    }
    return walle::StringRef(node_name);
}
class N_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<cache_node_name();
    }
};

class U_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<msg.file;
    }
};

class u_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &tm)
    {
        msg.formatted<<msg.line;
    }
};

class w_formatter : public flag_formatter {
public:
    w_formatter()
    :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('w'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class W_formatter : public flag_formatter {
public:
    W_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('W'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class b_formatter : public flag_formatter {
public:
    b_formatter( )
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('b'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
       _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class B_formatter : public flag_formatter {
public:
    B_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('B'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};



class d_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &dt)
    {
        walle::flag_formatter_map::get_instance()->get_flag_formatter('d')->format(msg.formatted, dt);
    }
};

class e_formatter : public flag_formatter {
public:
    e_formatter()
    :_ptr( walle::flag_formatter_map::get_instance()->get_flag_formatter('e'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
       _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class f_formatter : public flag_formatter {
public:
    f_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('f'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
         _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class m_formatter : public flag_formatter {
public:
    m_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('m'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class n_formatter : public flag_formatter {
public:
    void format(Message &msg, walle::DateTime &dt)
    {
         walle::flag_formatter_map::get_instance()->get_flag_formatter('n')->format(msg.formatted, dt);
    }
};

class o_formatter : public flag_formatter {
public:
    o_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('o'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class y_formatter : public flag_formatter {
public:
    y_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('y'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class Y_formatter : public flag_formatter {
public:
    Y_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('Y'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
         _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class H_formatter : public flag_formatter {
public:
    H_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('H'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};


class h_formatter : public flag_formatter {
public:
    h_formatter()
        :_ptr( walle::flag_formatter_map::get_instance()->get_flag_formatter('h'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
       _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class a_formatter : public flag_formatter {
public:
    a_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('a'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
       _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class A_formatter : public flag_formatter {
public:
    A_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('A'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
       _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class M_formatter : public flag_formatter {
public:
    M_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('M'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class S_formatter : public flag_formatter {
public:
    S_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('S'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class i_formatter : public flag_formatter {
public:
    i_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('i'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
       _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class F_formatter : public flag_formatter {
public:
    F_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('F'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class z_formatter : public flag_formatter {
public:
    z_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('z'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class Z_formatter : public flag_formatter {
public:
    Z_formatter()
        :_ptr(walle::flag_formatter_map::get_instance()->get_flag_formatter('Z'))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        _ptr->format(msg.formatted, dt);
    }
private:
    walle::flag_formatter_ptr _ptr;
};

class k_formatter : public flag_formatter {
public:
    char _c;
    walle::flag_formatter_ptr ptr;
    k_formatter(char c)
        :_c(c),
        ptr(walle::flag_formatter_map::get_instance()->get_char_formatter(_c))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        ptr->format(msg.formatted, dt);
    }
};

class K_formatter : public flag_formatter {
public:
    std::string _str;
    walle::flag_formatter_ptr ptr;
    K_formatter(const std::string &str): 
    _str(str),
    ptr(walle::flag_formatter_map::get_instance()->get_string_formatter(_str))
    {

    }
    void format(Message &msg, walle::DateTime &dt)
    {
        ptr->format(msg.formatted, dt);
    }
};

log_flag_formatter_map::log_flag_formatter_map()
{
    flag_formatters.clear();
    flag_formatters.resize(256);
    flag_formatters[static_cast<size_t>('V')] =log_flag_formatter_ptr(new V_formatter());
    flag_formatters[static_cast<size_t>('s')] =log_flag_formatter_ptr(new s_formatter());
    flag_formatters[static_cast<size_t>('v')] =log_flag_formatter_ptr(new v_formatter());
    flag_formatters[static_cast<size_t>('l')] =log_flag_formatter_ptr(new l_formatter());
    flag_formatters[static_cast<size_t>('p')] =log_flag_formatter_ptr(new p_formatter());
    flag_formatters[static_cast<size_t>('q')] =log_flag_formatter_ptr(new q_formatter());
    flag_formatters[static_cast<size_t>('P')] =log_flag_formatter_ptr(new P_formatter());
    flag_formatters[static_cast<size_t>('T')] =log_flag_formatter_ptr(new T_formatter());
    flag_formatters[static_cast<size_t>('I')] =log_flag_formatter_ptr(new I_formatter());
    flag_formatters[static_cast<size_t>('N')] =log_flag_formatter_ptr(new N_formatter());
    flag_formatters[static_cast<size_t>('u')] =log_flag_formatter_ptr(new u_formatter());
    flag_formatters[static_cast<size_t>('U')] =log_flag_formatter_ptr(new U_formatter());
    flag_formatters[static_cast<size_t>('w')] =log_flag_formatter_ptr(new w_formatter());
    flag_formatters[static_cast<size_t>('W')] =log_flag_formatter_ptr(new W_formatter());
    flag_formatters[static_cast<size_t>('b')] =log_flag_formatter_ptr(new b_formatter());
    flag_formatters[static_cast<size_t>('B')] =log_flag_formatter_ptr(new B_formatter());
    flag_formatters[static_cast<size_t>('d')] =log_flag_formatter_ptr(new d_formatter());
    flag_formatters[static_cast<size_t>('e')] =log_flag_formatter_ptr(new e_formatter());
    flag_formatters[static_cast<size_t>('f')] =log_flag_formatter_ptr(new f_formatter());
    flag_formatters[static_cast<size_t>('m')] =log_flag_formatter_ptr(new m_formatter());
    flag_formatters[static_cast<size_t>('n')] =log_flag_formatter_ptr(new n_formatter());
    flag_formatters[static_cast<size_t>('o')] =log_flag_formatter_ptr(new o_formatter());
    flag_formatters[static_cast<size_t>('y')] =log_flag_formatter_ptr(new y_formatter());
    flag_formatters[static_cast<size_t>('Y')] =log_flag_formatter_ptr(new Y_formatter());
    flag_formatters[static_cast<size_t>('H')] =log_flag_formatter_ptr(new H_formatter());
    flag_formatters[static_cast<size_t>('h')] =log_flag_formatter_ptr(new h_formatter());
    flag_formatters[static_cast<size_t>('a')] =log_flag_formatter_ptr(new a_formatter());
    flag_formatters[static_cast<size_t>('A')] =log_flag_formatter_ptr(new A_formatter());
    flag_formatters[static_cast<size_t>('M')] =log_flag_formatter_ptr(new M_formatter());
    flag_formatters[static_cast<size_t>('S')] =log_flag_formatter_ptr(new S_formatter());
    //flag_formatters[static_cast<size_t>('k')] =log_flag_formatter_ptr(new k_formatter());
    //flag_formatters[static_cast<size_t>('K')] =log_flag_formatter_ptr(new K_formatter());
    flag_formatters[static_cast<size_t>('i')] =log_flag_formatter_ptr(new i_formatter());
    flag_formatters[static_cast<size_t>('F')] =log_flag_formatter_ptr(new F_formatter());
    flag_formatters[static_cast<size_t>('z')] =log_flag_formatter_ptr(new z_formatter());
    flag_formatters[static_cast<size_t>('Z')] =log_flag_formatter_ptr(new Z_formatter());

}

log_flag_formatter_map::~log_flag_formatter_map()
{

}

 log_flag_formatter_ptr log_flag_formatter_map::get_flag_formatter(char c)
 {
     return  flag_formatters[static_cast<size_t>(c)];
 }
log_flag_formatter_ptr log_flag_formatter_map::get_char_formatter(char p)
{
    return log_flag_formatter_ptr(new k_formatter(p));
}
log_flag_formatter_ptr log_flag_formatter_map::get_string_formatter(const std::string &str)
{
    return log_flag_formatter_ptr(new K_formatter(str));
}

PatternFormatter::PatternFormatter()
{

}

PatternFormatter::~PatternFormatter()
{

}

 bool PatternFormatter::set_property(const std::string &name, const std::string &value)
 {
    if (name == "format") {
        _pattern = value;
        return compile_pattern();
    } 
    return false;
}
bool PatternFormatter::get_property(const std::string &name, std::string &value)
{
    if (name == "format") {
        value = _pattern;
        return true;
    }
    return false;
}

void PatternFormatter::format(Message &msg)
{
    FlagFormatterVecItr end = _flag_formatters.end();
    FlagFormatterVecItr it  = _flag_formatters.begin();
    walle::DateTime dt(msg.timestamp);
    if(_local) {
        dt.make_local(walle::get_local_tzd().tzd);
    }
    for(; it != end; it++) {
        (*it)->format(msg, dt);
    }
    msg.formatted<<"\r\n";
}

bool PatternFormatter::compile_pattern()
{
    std::string::iterator end = _pattern.end();
    std::string::iterator it = _pattern.begin();
    std::string user_char;
    for(; it != end; ++it) {
        if (*it == '%') {
            if(++it != end) {
                if(*it == '%') {
                    user_char.append(1, *it);
                    continue;
                }
                if (user_char.size() > 0 ) {
                    log_flag_formatter_ptr ptr(new K_formatter(user_char));
                     
                    _flag_formatters.push_back(ptr);
                    user_char.clear();
                }
                bool ret = handle_char(*it);
                if(!ret) {
                    return false;
                }
            } else {
                break;
            }

        } else {
            user_char.append(1, *it);
        }
    }
    if (user_char.size() > 0 ) {
        log_flag_formatter_ptr ptr(new K_formatter(user_char));
        _flag_formatters.push_back(ptr);
        user_char.clear();
    }
    return true;

}

bool PatternFormatter::handle_char(char c)
{
    log_flag_formatter_map *pmap = log_flag_formatter_map::get_instance();
    switch(c) {
        case 'V': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c)); 
            break;
        case 's': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c)); 
            break;
        case 'v': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'l': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'p': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'q': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'P': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'T': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'I': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'N': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'U': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'u': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'w': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'W': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'b': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'B': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'd': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'e': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'f': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'm': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'n': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'o': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'y': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'Y': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'H': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'h': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'a': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'A': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'M': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'S': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'i': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'F': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'z': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        case 'Z': 
            _flag_formatters.push_back(pmap->get_flag_formatter(c));
            break;
        default:
            return false;
    }
    return true;
}

} //namespace log
} //namespsace walle