#include "yaml_structs.h"

template <typename Type>
void operator >> (const YAML::Node& node, Type& value)
{
    if (node.IsDefined() && node.Type() == YAML::NodeType::Scalar)
    {
        value = node.as<Type>();
    }
}

void operator >> (const YAML::Node& node, QString& value)
{
    if (node.IsDefined() && node.Type() == YAML::NodeType::Scalar)
    {
        value = node.as<std::string>().c_str();
    }
}


void operator >> (const YAML::Node& node, description& desc)
{
    node["generic"] >> desc.m_generic;
}

void operator >> (const YAML::Node& nodes, products& prod)
{
    if (nodes.Type() == YAML::NodeType::Sequence)
    {
        for (auto node : nodes)
        {
            node["brand"]       >> prod.m_brand;
            node["description"] >> prod.m_description;
        }
    }
}

void operator >> (const YAML::Node& node, choices& s_choices)
{
    if (node.IsDefined() && node.Type() == YAML::NodeType::Sequence)
    {
        s_choices.m_choice.resize(node.size());
        int i=0;
        for (const auto& choice : node)
        {
            choice >> s_choices.m_choice[i++];
        }
    }
}

void operator >> (const YAML::Node& nodes, parameters& s_parameters)
{
    if (nodes.Type() == YAML::NodeType::Sequence)
    {
        for (const auto &node: nodes)
        {
            QString name;
            node["name"] >> name;
            parameters::parameter param;
            node["choice"]   >> param.m_choice;
            node["type"]     >> param.m_type;
            node["default"]  >> param.m_default;
            node["advanced"] >> param.m_advanced;
            node["allinone"] >> param.m_allinone;
            s_parameters.m_map[name] = param;
        }
    }
}

QString parameters::get_choice(const QString& name, int n)
{
    if (m_map.contains(name) && n < static_cast<int>(m_map[name].m_choice.m_choice.size()))
    {
        return m_map[name].m_choice.m_choice[n];
    }
    return "";
}

QString parameters::get_type(const QString& name)
{
    if (m_map.contains(name))
    {
        return m_map[name].m_type;
    }
    return "";
}
QString parameters::get_default(const QString& name)
{
    if (m_map.contains(name))
    {
        return m_map[name].m_default;
    }
    return "";
}

bool parameters::is_allinone(const QString& name)
{
    if (m_map.contains(name))
    {
        return m_map[name].m_allinone;
    }
    return false;
}

bool parameters::is_advanced(const QString& name)
{
    if (m_map.contains(name))
    {
        return m_map[name].m_advanced;
    }
    return false;
}

void operator >> (const YAML::Node& nodes, measured_register& _register)
{
    QString storageunit;
    for (const auto &node: nodes)
    {
        auto key = node.first.as<std::string>();
        const YAML::Node& value = node.second;
        if (key == "command")
        {
            QString thecommand;
            value >> thecommand;
            if (thecommand.indexOf("end") != -1)
            {
                storageunit.clear();
            }
            else
            {
                auto pos1   = thecommand.indexOf("\"");
                auto pos2   = thecommand.lastIndexOf("\"");
                storageunit = thecommand.mid(pos1+1, pos2-pos1-1);
            }
        }
        else if (key == "address")
        {
            if (!storageunit.isEmpty())
            {
                QString address;
                nodes["address"] >> address;
                _register.m_address += address+" "+storageunit+";";
            }
            else
            {
                nodes["address"] >> _register.m_address;
            }
        }
        else
        {
            nodes["type"]    >> _register.m_type;
            nodes["decode"]  >> _register.m_decode;
        }
    }
}


void operator >> (const YAML::Node& nodes, measured_value& _measured_value)
{
    nodes["model"]        >> _measured_value.m_model;
    nodes["timeout"]      >> _measured_value.m_timeout;
    nodes["connectdelay"] >> _measured_value.m_connectdelay;
    nodes["register"]     >> _measured_value.m_register;
    nodes["scale"]        >> _measured_value.m_scale;
}

void s_render::read_measurement(const YAML::Node& node, const QString& name)
{
    if (node.IsDefined() && current_usage.size())
    {
        QString full_name = current_usage+":";
        full_name+=name;
        if (node.IsSequence())
        {
            char c = '1';
            for (auto& node : node)
            {
                measured_value mv;
                node >> mv;
                m_measurements[full_name+c] = mv;
                c++;
            }
        }
        else
        {
            measured_value mv;
            node >> mv;
            m_measurements[full_name] = mv;
        }
    }
}

bool s_render::get_command()
{
    if (last_command.indexOf("end") != -1)
    {
        return false;
    }
    auto pos1 = last_command.indexOf("\"");
    auto pos2 = last_command.lastIndexOf("\"");
    if (pos1 != -1 && pos2 != -1)
    {
        if (last_command.indexOf("usage") != -1)
        {
            current_usage = last_command.mid(pos1+1, pos2-pos1-1);
        }
    }
    return true;
}

bool s_render::read_key(const QString& key, const YAML::Node& node)
{
    if (key == "type")
    {
        node >> m_type;
    }
    else if (key == "capacity")
    {
        /// TODO: what?
    }
    else if (key == "command")
    {
        node >> last_command;
        if (get_command())
        {
            command++;
        }
        else
        {
            command--;
        }
    }
    else
    {
        read_measurement(node, key);
    }
    return command >= 0;
}

void operator >> (const YAML::Node& nodes, s_render& _render)
{
    for (const auto &node: nodes)
    {
        if (!_render.read_key(node.first.as<std::string>().c_str(), node.second))
        {
            break;
        }
    }
}

void operator >> (const YAML::Node& node, meter& _meter)
{
    node["template"] >> _meter.m_template;
    node["products"] >> _meter.m_products;
    node["params"]   >> _meter.m_parameters;
    node["render"]   >> _meter.m_render_source;
}


