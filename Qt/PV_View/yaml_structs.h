#ifndef YAML_STRUCTS_H
#define YAML_STRUCTS_H

#include <QString>
#include <QMap>

#include <yaml-cpp/yaml.h>


struct description
{
    QString m_generic;

    friend void operator >> (const YAML::Node& node, description& desc);
};

struct products
{
    QString m_brand;
    description m_description;

    friend void operator >> (const YAML::Node& nodes, products& prod);
};

struct choices
{
    std::vector<QString> m_choice;

    friend void operator >> (const YAML::Node& node, choices& s_choices);
};


struct parameters
{
    struct parameter
    {
        choices m_choice;
        QString m_type;
        QString m_default;
        bool    m_advanced = false;
        bool    m_allinone = false;
    };
    friend void operator >> (const YAML::Node& nodes, parameters& s_parameters);

    QString get_choice(const QString& name, int n=0);
    QString get_type(const QString& name);
    QString get_default(const QString& name);
    bool    is_allinone(const QString& name);
    bool    is_advanced(const QString& name);
private:
    QMap<QString, parameter> m_map;
};

struct measured_register
{
    QString m_address;
    QString m_type;
    QString m_decode;

    friend void operator >> (const YAML::Node& nodes, measured_register& _register);
};

struct measured_value
{
    QString m_model;
    QString m_timeout;
    QString m_connectdelay;
    measured_register m_register;
    double      m_scale = 1;

    friend void operator >> (const YAML::Node& nodes, measured_value& _measured_value);
};

typedef QMap<QString, measured_value> measurement_map;

struct s_render
{
    QString m_type;
    measurement_map m_measurements;

private:
    QString last_command;
    QString current_usage;
    int         command = 0;

    void read_measurement(const YAML::Node& node, const QString& name);
    bool get_command();
    bool read_key(const QString& key, const YAML::Node& node);

    friend void operator >> (const YAML::Node& nodes, s_render& _render);
};

struct meter
{
    QString m_template;
    products    m_products;
    parameters  m_parameters;
    QString m_render_source;
    s_render    m_rendered;

    friend void operator >> (const YAML::Node& node, meter& met);
};



#endif // YAML_STRUCTS_H
