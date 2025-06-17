#ifndef BYY_TRITON_OPTIONS
#define BYY_TRITON_OPTIONS 1

#include <osgEarthUtil/Ocean>


class /*header-only*/ ByyTritonOptions : public osgEarth::Util::OceanOptions
{
public:
    ByyTritonOptions(const osgEarth::Util::OceanOptions& conf = osgEarth::Util::OceanOptions()) :
        osgEarth::Util::OceanOptions( conf )
    {
        setDriver( "triton" );
        _useHeightMap.init( true );
        _heightMapSize.init( 1024 );
        _renderBinNumber.init( 1 );
        fromConfig( _conf );
    }

    virtual ~ByyTritonOptions() { }

    /* User name for license activation */
    osgEarth::optional<std::string>& user() { return _user; }
    const osgEarth::optional<std::string>& user() const { return _user; }

    /* License code string */
    osgEarth::optional<std::string>& licenseCode() { return _licenseCode; }
    const osgEarth::optional<std::string>& licenseCode() const { return _licenseCode; }

    /* Triton resource path */
    osgEarth::optional<std::string>& resourcePath() { return _resourcePath; }
    const osgEarth::optional<std::string>& resourcePath() const { return _resourcePath; }

    /** Whether to pass a heightmap raster to Triton to mitigate coastline interference */
    osgEarth::optional<bool>& useHeightMap() { return _useHeightMap; }
    const osgEarth::optional<bool>& useHeightMap() const { return _useHeightMap; }

    /** Texture size to use for the height map */
    osgEarth::optional<int>& heightMapSize() { return _heightMapSize; }
    const osgEarth::optional<int>& heightMapSize() const { return _heightMapSize; }

    /** Render bin number to assign to the ocean (in DepthSortedBin) */
    osgEarth::optional<int>& renderBinNumber() { return _renderBinNumber; }
    const osgEarth::optional<int>& renderBinNumber() const { return _renderBinNumber; }


public:
    osgEarth::Config getConfig() const {
        osgEarth::Config conf = osgEarth::Util::OceanOptions::newConfig();
        conf.addIfSet("user", _user);
        conf.addIfSet("license_code", _licenseCode);
        conf.addIfSet("resource_path", _resourcePath);
        conf.addIfSet("use_height_map", _useHeightMap);
        conf.addIfSet("height_map_size", _heightMapSize);
        conf.addIfSet("render_bin_number",   _renderBinNumber);
        return conf;
    }

protected:
    void mergeConfig( const osgEarth::Config& conf ) {
        osgEarth::Util::OceanOptions::mergeConfig( conf );
        fromConfig( conf );
    }

private:
    void fromConfig( const osgEarth::Config& conf ) {
        conf.getIfSet("user", _user);
        conf.getIfSet("license_code", _licenseCode);
        conf.getIfSet("resource_path", _resourcePath);
        conf.getIfSet("use_height_map", _useHeightMap);
        conf.getIfSet("height_map_size", _heightMapSize);
        conf.getIfSet("render_bin_number",   _renderBinNumber);
    }

private:
    osgEarth::optional<std::string> _user;
    osgEarth::optional<std::string> _licenseCode;
    osgEarth::optional<std::string> _resourcePath;
    osgEarth::optional<bool>        _useHeightMap;
    osgEarth::optional<int>         _heightMapSize;
    osgEarth::optional<int>         _renderBinNumber;
};

#endif // BYY_TRITON_OPTIONS
