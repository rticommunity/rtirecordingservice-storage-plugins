/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include <algorithm>

#include <rti/util/StreamFlagSaver.hpp>
#include "UtilsStorageWriter.hpp"
#include "PrintFormatCsv.hpp"
#include "Logger.hpp"

#define NANOSECS_PER_SEC 1000000000ll

/*
 * --- UtilsStorageProperty ---------------------------------------------------
 */

namespace rti { namespace recorder { namespace utils {

UtilsStorageProperty::UtilsStorageProperty()
    : output_dir_path_("."),
      merge_output_files_(false),
      output_format_kind_(OutputFormatKind::CSV_FORMAT)
{
}

UtilsStorageProperty& UtilsStorageProperty::output_dir_path(const std::string& path)
{
    output_dir_path_ = path;

    return *this;
}

UtilsStorageProperty& UtilsStorageProperty::output_file_basename(const std::string& output_file_basename)
{
    output_file_basename_ = output_file_basename;

    return *this;
}


UtilsStorageProperty& UtilsStorageProperty::output_format_kind(const OutputFormatKind& kind)
{
    output_format_kind_ = kind;

    return *this;
}

UtilsStorageProperty& UtilsStorageProperty::merge_output_files(bool merge)
{
    merge_output_files_ = merge;

    return *this;
}

std::string UtilsStorageProperty::output_dir_path() const
{
    return output_dir_path_;
}

std::string UtilsStorageProperty::output_file_basename() const
{
    return output_file_basename_;
}

OutputFormatKind UtilsStorageProperty::output_format_kind() const
{
    return output_format_kind_;
}

bool UtilsStorageProperty::merge_output_files() const
{
    return merge_output_files_;
}

std::ostream& operator<<(
        std::ostream& os,
        const UtilsStorageProperty& property)
{
    rti::util::StreamFlagSaver stream_flag_saver(os);

    size_t namespace_length =
            UtilsStorageWriter::PROPERTY_NAMESPACE().length() + 1;
    os << "\t"
            <<  UtilsStorageWriter::OUTPUT_DIR_PROPERTY_NAME().substr(namespace_length)
            << "="
            << property.output_dir_path()
            << "\n";

    os << "\t" <<
            UtilsStorageWriter::OUTPUT_FILE_BASENAME_PROPERTY_NAME().substr(namespace_length)
            << "="
            << property.output_file_basename()
            << "\n";

    os << "\t" <<
            UtilsStorageWriter::OUTPUT_MERGE_PROPERTY_NAME().substr(namespace_length)
            << "="
            << std::boolalpha << property.merge_output_files()
            << "\n";

    os << "\t" <<
            UtilsStorageWriter::LOGGING_VERBOSITY_PROPERTY_NAME().substr(namespace_length)
            << "="
            << Logger::instance().verbosity()
            << "\n";

    return os;
}

std::ostream& operator<<(
        std::ostream& os,
        const PrintFormatCsvProperty& property)
{
    // Add CSV property
    size_t namespace_length =
            UtilsStorageWriter::PROPERTY_NAMESPACE().length() + 1;
    os << "\t" <<
            UtilsStorageWriter::CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME().substr(namespace_length)
            << "="
            << property.empty_member_value_representation()
            << "\n";

    os << "\t" <<
            UtilsStorageWriter::CSV_ENUM_AS_STRING_PROPERTY_NAME().substr(namespace_length)
            << "="
            << std::boolalpha << property.enum_as_string();

    return os;
}


/*
 * --- UtilsStorageWriter ---------------------------------------------------
 */

RTI_RECORDING_STORAGE_WRITER_CREATE_DEF(UtilsStorageWriter);


const dds::core::xtypes::DynamicType& dynamic_type(
        const rti::routing::StreamInfo& stream_info)
{
    assert(stream_info.type_info().type_representation_kind()
            == rti::routing::TypeRepresentationKind::DYNAMIC_TYPE);
    return *(static_cast<dds::core::xtypes::DynamicType*>(
            stream_info.type_info().type_representation()));
}

const std::vector<char>& reserved_filename_chars()
{
    static std::vector<char> value;
    static bool is_initialized = false;

    if (!is_initialized) {
        value.push_back('<');
        value.push_back('>');
        value.push_back(':');
        value.push_back('\'');
        value.push_back('/');
        value.push_back('\\');
        value.push_back('|');
        value.push_back('?');
        value.push_back('*');
    }

    return value;
}

const std::string& UtilsStorageWriter::PROPERTY_NAMESPACE()
{
    static const std::string value = "rti.recording.utils_storage";
    return value;
}


const std::string& UtilsStorageWriter::OUTPUT_DIR_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
            + ".output_dir_path";
    return value;
}

const std::string& UtilsStorageWriter::OUTPUT_FILE_BASENAME_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
        + ".output_file_basename";
    return value;
}


const std::string& UtilsStorageWriter::OUTPUT_FORMAT_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
            + ".output_format";
    return value;
}

const std::string& UtilsStorageWriter::OUTPUT_MERGE_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
            + ".merge_output_files";
    return value;
}

const std::string& UtilsStorageWriter::LOGGING_VERBOSITY_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
            + ".verbosity";
    return value;
}

const std::string& UtilsStorageWriter::CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
            + ".csv.empty_member_value";
    return value;
}

const std::string& UtilsStorageWriter::CSV_ENUM_AS_STRING_PROPERTY_NAME()
{
    static const std::string value = PROPERTY_NAMESPACE()
            + ".csv.enum_as_string";
    return value;
}


const std::string& UtilsStorageWriter::CSV_FILE_EXTENSION()
{
    static const std::string value = ".csv";
    return value;
}

const std::string& UtilsStorageWriter::OUTPUT_FILE_BASENAME_DEFAULT()
{
    static const std::string value = "csv_converted";
    return value;
}


char UtilsStorageWriter::FILE_NAME_REPLACEMENT_CHAR()
{
    static char value = '#';

    return value;
}


/*
 * @brief Helper for the static initialization of the constant default
 * values for a UtilsStorageProperty.
 *
 * Instead of declaring a constructor that takes all the parameters, this
 * patterns allows to set each member individually through a setter within
 * the constructor of this initializer class. Then an object of this class
 * can be statically initialized and guarantee atomic construction as per the
 * C++ conditions.
 */
struct UtilsStoragePropertDefaultInitializer {

    UtilsStoragePropertDefaultInitializer()
    {
        value.merge_output_files(true);
        value.output_dir_path(".");
        value.output_file_basename(UtilsStorageWriter::OUTPUT_FILE_BASENAME_DEFAULT());
        value.output_format_kind(OutputFormatKind::CSV_FORMAT);
    }
    UtilsStorageProperty value;
};

const UtilsStorageProperty& UtilsStorageWriter::PROPERTY_DEFAULT()
{
    static UtilsStoragePropertDefaultInitializer property;
    return property.value;
}


/*
 * In the xml configuration, under the property tag for the storage plugin, a
 * collection of name/value pairs can be passed. In this case, this example
 * chooses to define a property to name the filename to use.
 */
UtilsStorageWriter::UtilsStorageWriter(
        const rti::routing::PropertySet& properties) :
    StorageWriter(properties),
    property_(PROPERTY_DEFAULT()),
    csv_property_(PrintFormatCsv::PROPERTY_DEFAULT())
{

    // verbosity
    rti::routing::PropertySet::const_iterator found =
            properties.find(LOGGING_VERBOSITY_PROPERTY_NAME());
    if (found != properties.end()) {
        rti::config::Verbosity verbosity =
                rti::config::Verbosity::EXCEPTION;
        // convert to verbosity level
        switch (std::stoi(found->second)) {
        case 0:
            verbosity = rti::config::Verbosity::SILENT;
            break;
        case 1:
            verbosity = rti::config::Verbosity::EXCEPTION;
            break;
        case 2:
            verbosity = rti::config::Verbosity::WARNING;
            break;
        case 3:
            verbosity = rti::config::Verbosity::STATUS_LOCAL;
            break;
        case 4:
            verbosity = rti::config::Verbosity::STATUS_REMOTE;
            break;
        default:
            verbosity = rti::config::Verbosity::STATUS_ALL;
            break;
        }
        Logger::instance().verbosity(verbosity);
    }

    // output directory
    found = properties.find(OUTPUT_DIR_PROPERTY_NAME());
    if (found != properties.end()) {
        property_.output_dir_path(found->second);
    }

    // output file base name
    found = properties.find(OUTPUT_FILE_BASENAME_PROPERTY_NAME());
    if (found != properties.end()) {
        property_.output_file_basename(found->second);
    }

    // output format
    found = properties.find(OUTPUT_FORMAT_PROPERTY_NAME());
    if (found != properties.end()) {
        if (found->second != "CSV") {
            throw dds::core::UnsupportedError(
                    "unsupported output format=" + found->second);
        }
    }

    // merge output files
    found = properties.find(OUTPUT_MERGE_PROPERTY_NAME());
    if (found != properties.end()) {
        std::istringstream bool_as_string(found->second);
        bool value = false;
        try {
            bool_as_string >> std::boolalpha >> value;
        } catch (const std::exception& ex) {
            throw dds::core::Error(
                    std::string(ex.what())
                    + ". Invalid value for property with name="
                    + OUTPUT_MERGE_PROPERTY_NAME()
                    + ": valid values are 'true' or 'false'");
        }
        property_.merge_output_files(value);

    }
    if (property_.merge_output_files()) {
        // build output file name
        std::string output_file_name =
                property_.output_dir_path()
                + RTI_RECORDER_UTILS_PATH_SEPARATOR
                + property_.output_file_basename()
                + CSV_FILE_EXTENSION();
        output_merged_file_.open(output_file_name);
        if (!output_merged_file_.good()) {
            throw dds::core::Error("failed to open output merged file="
                    + output_file_name);
        }
    }

    // CSV-specific properties
    // representation for the empty member
    found = properties.find(CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME());
    if (found != properties.end()) {
        csv_property_.empty_member_value_representation(found->second);
    }

    // print enumerations as strings
    found = properties.find(CSV_ENUM_AS_STRING_PROPERTY_NAME());
    if (found != properties.end()) {
        std::istringstream bool_as_string(found->second);
        bool value = false;
        try {
            bool_as_string >> std::boolalpha >> value;
        } catch (const std::exception& ex) {
            throw dds::core::Error(
                    std::string(ex.what())
                    + ". Invalid value for property with name="
                    + CSV_ENUM_AS_STRING_PROPERTY_NAME()
                    + ": valid values are 'true' or 'false'");
        }
        csv_property_.enum_as_string(value);
    }

    /* Log summary of configuration */
    if (Logger::instance().verbosity().underlying()
            >= rti::config::Verbosity::STATUS_LOCAL) {
        std::ostringstream summary;

        summary << "Utils Storage plug-in configuration:" << "\n";
        summary << property_;
        summary << csv_property_;

        RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::STATUS_LOCAL,
                    summary.str().c_str());
    }
}

UtilsStorageWriter::~UtilsStorageWriter()
{
    if (property_.merge_output_files()) {
        RTI_RECORDER_UTILS_LOG_MESSAGE(
                rti::config::Verbosity::STATUS_LOCAL,
                "UtilsStorageWriter: delete output files after merge");
        for (auto it = output_files_.begin();
                it != output_files_.end();
                ++it) {
            if (std::remove(it->first.c_str()) != 0) {
                RTI_RECORDER_UTILS_LOG_MESSAGE(
                        rti::config::Verbosity::EXCEPTION,
                        "error deleting output file=" + it->first+
                        + " with error code=" + std::to_string(errno));
            }
        }
    }
}

rti::recording::storage::StorageStreamWriter *
UtilsStorageWriter::create_stream_writer(
        const rti::routing::StreamInfo& stream_info,
        const rti::routing::PropertySet&)
{
    std::string output_file_name =
            property_.output_file_basename()
            + "-"
            + stream_info.stream_name();
    // replace any invalid character
    for (auto &reserved_char : reserved_filename_chars()) {
        std::replace(
                output_file_name.begin(),
                output_file_name.end(),
                reserved_char,
                FILE_NAME_REPLACEMENT_CHAR());
    }
    std::string output_file_path =
            property_.output_dir_path()
            + RTI_RECORDER_UTILS_PATH_SEPARATOR
            + output_file_name
            + CSV_FILE_EXTENSION();
    std::ofstream output_file;
    output_file.open(output_file_path.c_str(), std::ios::out);
    if (!output_file.good()) {
        throw dds::core::Error(
                "Failed to open file="
                + output_file_path
                + " to store data samples for stream with name="
                + stream_info.stream_name());
    }
    // Write table header
    output_file << "Topic name: " << stream_info.stream_name() << std::endl;
    // add to collection
    output_files_.insert(std::make_pair(
            output_file_path,
            std::move(output_file)));

    RTI_RECORDER_UTILS_LOG_MESSAGE(
            rti::config::Verbosity::STATUS_LOCAL,
            ("UtilsStorageWriter: create StreamWriter for file=" + output_file_path).c_str());

    switch(property_.output_format_kind()) {

    case OutputFormatKind::CSV_FORMAT:
    {
        return new CsvStreamWriter(
                csv_property_,
                stream_info,
                *(output_files_.find(output_file_path)));
    }
        break;

    default:
        throw dds::core::UnsupportedError("unsupported output format kind");
    };

}

void UtilsStorageWriter::delete_stream_writer(
        rti::recording::storage::StorageStreamWriter *writer)
{
    CsvStreamWriter *stream_writer = static_cast<CsvStreamWriter*> (writer);
    RTI_RECORDER_UTILS_LOG_MESSAGE(
            rti::config::Verbosity::STATUS_LOCAL,
            ("UtilsStorageWriter: delete StreamWriter for file="
                    + stream_writer->file_entry().first).c_str());
    try {
        stream_writer->file_entry().second.flush();
        if (property_.merge_output_files()) {
             RTI_RECORDER_UTILS_LOG_MESSAGE(
                    rti::config::Verbosity::STATUS_LOCAL,
                    ("UtilsStorageWriter: merge file="
                            + stream_writer->file_entry().first).c_str());
            merge_output_file(stream_writer->file_entry());
        }
        stream_writer->file_entry().second.close();
    } catch (const std::exception& ex) {
        RTI_RECORDER_UTILS_LOG_MESSAGE(
                rti::config::Verbosity::EXCEPTION,
                ex.what());
    } catch (...) {
        std::string message =
                "unexpected exception occurred while merging file with name="
                + stream_writer->file_entry().first;
        RTI_RECORDER_UTILS_LOG_MESSAGE(
                rti::config::Verbosity::EXCEPTION,
                message);
    }

    delete writer;
}

void UtilsStorageWriter::merge_output_file(
        FileSetEntry& file_entry)
{
    std::ifstream input_file(file_entry.first);
    std::string line;
    while (std::getline(input_file, line)) {
        output_merged_file_ << line << std::endl;
    }
}


/*
 * --- CsvStreamWriter --------------------------------------------------------
 */

CsvStreamWriter::CsvStreamWriter(
            const PrintFormatCsvProperty& property,
            const rti::routing::StreamInfo& stream_info,
            UtilsStorageWriter::FileSetEntry& output_file_entry) :
    output_file_entry_(output_file_entry),
    print_format_csv_(
            property,
            dynamic_type(stream_info),
            output_file_entry.second)
{
}

CsvStreamWriter::~CsvStreamWriter()
{

}

/*
 * This function is called by Recorder whenever there are samples available for
 * one of the streams previously discovered and accepted (see the
 * FileStorageWriter_create_stream_writer() function below). Recorder provides
 * the samples and their associated information objects in Routing Service
 * format, this is, untyped format.
 * In our case we know that, except for the built-in DDS discovery topics which
 * are received in their own format - and that we're not really storing -, that
 * the format of the data we're receiving is DDS Dynamic Data. This will always
 * be the format received for types recorded from DDS.
 * The function traverses the collection of samples and stores the data in a
 * textual format.
 */
void CsvStreamWriter::store(
        const std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
        const std::vector<dds::sub::SampleInfo *>& info_seq)
{
    using namespace dds::core::xtypes;
    using namespace rti::core::xtypes;
    using namespace dds::sub;

    const int32_t count = sample_seq.size();
    for (int32_t i = 0; i < count; ++i) {
        const SampleInfo& sample_info = *(info_seq[i]);
        // we first first print the sample's metadata
        int64_t timestamp =
                (int64_t) sample_info->reception_timestamp().sec()
                * NANOSECS_PER_SEC;
        timestamp += sample_info->reception_timestamp().nanosec();

        // print sample data
        if (sample_info->valid()) {
            DDS_UnsignedLong data_as_csv_length = 0;

            // compute required size
            DDS_ReturnCode_t native_retcode = DDS_DynamicDataFormatter_to_string_w_format(
                    &sample_seq[i]->native(),
                    NULL,
                    &data_as_csv_length,
                    print_format_csv_.native());
            rti::core::check_return_code(
                    native_retcode,
                    "failed to compute CSV data required length");
            data_as_csv_.resize(data_as_csv_length);
            native_retcode = DDS_DynamicDataFormatter_to_string_w_format(
                    &sample_seq[i]->native(),
                    &data_as_csv_[0],
                    &data_as_csv_length,
                    print_format_csv_.native());
            rti::core::check_return_code(
                    native_retcode,
                    "failed convert to DynamicData to CSV");

            // add timestamp metadata (first column)
            output_file_entry_.second << timestamp;

            // add formatted sample content to file
            output_file_entry_.second << data_as_csv_;
            // end of row
            output_file_entry_ .second << std::endl;
        }
    }
}

UtilsStorageWriter::FileSetEntry& CsvStreamWriter::file_entry()
{
    return output_file_entry_;
}

} } }

