#ifndef RPGSS_CPUINFO_HPP_INCLUDED
#define RPGSS_CPUINFO_HPP_INCLUDED


namespace rpgss {

    bool CpuSupportsMmx();
    bool CpuSupportsSse();
    bool CpuSupportsSse2();
    bool CpuSupportsSse3();

} // namespace rpgss


#endif // RPGSS_CPUINFO_HPP_INCLUDED
