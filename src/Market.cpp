#include "Market.h"
#include "CurveDiscount.h"

#include <vector>

namespace minirisk {
static bool is_prefix(const string& s1, const string& s2)
{
    const char*p = s1.c_str();
    const char*q = s2.c_str();
    while (*p && *q)
        if (*p++ != *q++)
            return false;
    return true;
}


ptr_disc_curve_t Market::build_discount_curve(const string& ccy)
{
    return ptr_disc_curve_t(new CurveDiscount(this, m_today, ccy));
}

double Market::from_mds(const string& objtype, const string& name)
{
    auto ins = m_data_points.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        ins.first->second = m_mds->get(name);
    }
    return ins.first->second;
}

const double Market::get_yield(const string& ccyname)
{
    string name("IR." + ccyname);
    return from_mds("yield curve", name);
};

const double Market::get_fx_spot(const string& ccy)
{
    string name("FX.SPOT." + ccy);
    return from_mds("fx spot", name);
}

void Market::set_data_points(const vec_risk_factor_t& data_points)
{
    clear();
    for (auto d = data_points.begin(); d != data_points.end(); ++d) {
        auto i = m_data_points.find(d->first);
        MYASSERT((i != m_data_points.end()), "Risk factor not found " << d->first);
        i->second = d->second;
    }
}

Market::vec_risk_factor_t Market::get_risk_factors(const std::string& prefix) const
{
    vec_risk_factor_t result;
    for (auto d = m_data_points.begin(), e = m_data_points.end(); d != e; ++d)
        if (is_prefix(prefix, d->first))
            result.push_back(*d);
    return result;
}
}
