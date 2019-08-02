#include "Configuration.h"
#include "boost/filesystem.hpp"

/// <summary>
/// Loads configuration YAML file, parses it and sets configuration values
/// </summary>
void Configuration::LoadConfigurationFile()
{
    // loading and parsing file
    try
    {
        if (boost::filesystem::exists(this->configurationFile)) // If user specific config exsits, load that
        {
            yamlParsed_ = YAML::LoadFile(this->configurationFile);
            // BOOST_LOG_TRIVIAL(info) << "Loaded specific configuration." << std::endl;
            std::cout << "Loaded specific configuration." << std::endl;
        }
        else // Otherwise load default config file
        {
            yamlParsed_ = YAML::LoadFile(this->configurationFileDefault);
            // BOOST_LOG_TRIVIAL(info) << "Loaded default configuration." << std::endl;
            std::cout << "Loaded default configuration." << std::endl;
        }
    }
    catch (YAML::ParserException&)
    {
        // BOOST_LOG_TRIVIAL(warning) << "Configuration file could not be parsed. Using default values." << std::endl;
        std::cerr << "Configuration file could not be parsed. Using default values." << std::endl;
    }
    catch (YAML::BadFile&)
    {
        // BOOST_LOG_TRIVIAL(warning) << "Configuration file could not found. Using default values." << std::endl;
        std::cerr << "Configuration file could not found. Using default values." << std::endl;
    }

    // setting particular YAML entries into configuration values
    this->SetupConfigurationValue("UsingGPU", this->usingGPU_);
    this->SetupConfigurationValue("UsingCompression", this->usingCompression_);
    this->SetupConfigurationValue("Dir", this->dir_);
    this->SetupConfigurationValue("DatabaseDir", this->databaseDir_);
    this->SetupConfigurationValue("BlockSize", this->blockSize_);
    this->SetupConfigurationValue("BlockCount", this->blockCount_);
    this->SetupConfigurationValue("GroupByBuckets", this->groupByBuckets_);
    this->SetupConfigurationValue("ListenIP", this->listenIP_);
    this->SetupConfigurationValue("ListenPort", this->listenPort_);
    this->SetupConfigurationValue("Timeout", this->timeout_);
    this->SetupConfigurationValue("GPUCachePercent", this->GPUCachePercent_);
}
