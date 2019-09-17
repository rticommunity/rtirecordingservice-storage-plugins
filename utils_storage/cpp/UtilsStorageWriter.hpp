/**
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
#ifndef RTI_RECORDER_UTILS_STORAGE_WRITER_HPP_
#define RTI_RECORDER_UTILS_STORAGE_WRITER_HPP_

#include <fstream>

#include "rti/recording/storage/StorageWriter.hpp"
#include "rti/recording/storage/StorageStreamWriter.hpp"
#include "rti/recording/storage/StorageDiscoveryStreamWriter.hpp"

#include "PrintFormatCsv.hpp"

namespace rti { namespace recorder { namespace utils {

#ifdef RTI_WIN32
    #define RTI_RECORDER_UTILS_PATH_SEPARATOR "\\"
#else
    #define RTI_RECORDER_UTILS_PATH_SEPARATOR "/"
#endif

/***
 * @brief Convenience macro to forward-declare the C-style function that will be
 * called by RTI Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_WRITER_CREATE_DECL(UtilsStorageWriter);

/**
 * @brief Definition of the support output formats.
 */
enum class OutputFormatKind {
        CSV_FORMAT
};

/**
 * @brief Configuration elements of the Utils storage plug-in.
 *
 */
class UtilsStorageProperty {
public:
    UtilsStorageProperty();

    /**
     * @brief Selects the format in which samples are stored in an output
     * text file.
     *
     * Default: CSV_FORMAT
     */
    UtilsStorageProperty& output_format_kind(const OutputFormatKind&);

    /**
     * @brief Gets the OutputFormatKind
     */
    OutputFormatKind output_format_kind() const;

    /**
     * @brief Selects the directory where the output files are placed.
     *
     * Default: .
     */
    std::string output_dir_path() const;

    /**
     * @brief Gets the output_dir_path
     */
    UtilsStorageProperty& output_dir_path(const std::string&);

    /**
     * @brief Specifies a prefix for the output files.
     *
     * Default: [empty]
     */
    std::string output_file_basename() const;

    /**
     * @brief Gets the output_file_basename
     */
    UtilsStorageProperty& output_file_basename(const std::string&);

    /**
     * @brief Indicates whether the conversion of samples is placed into a
     * single file.
     *
     * Default: true
     */
    bool merge_output_files() const;

    /**
     * @brief Gets the merge_output_files
     */
    UtilsStorageProperty& merge_output_files(bool);

private:
    OutputFormatKind output_format_kind_;
    std::string output_dir_path_;
    std::string output_file_basename_;
    bool merge_output_files_;
};

/***
 * @brief String representation of UtilsStorageProperty
 */
std::ostream& operator<<(
        std::ostream& os,
        const UtilsStorageProperty& property);

/**
 * @brief String representation of PrintFormatCsvProperty
 */
std::ostream& operator<<(
        std::ostream& os,
        const PrintFormatCsvProperty& property);

/**
 * @brief Implementation of a StorageWriter plug-in that allows storing
 * DynamicData samples represented in a text-compatible format, such as CSV.
 *
 * In this implementation, each StreamWriter is responsible for converting
 * and storing the samples of the associated Stream/Topic into a dedicated
 * output text file, with attributes specified in UtilsStorageProperty.
 * All the output files the StreamWriters generate can be merged into a single
 * output file while deleting all the intermediate separate output files.
 *
 * @override rti::recording::storage::StorageWriter
 */
class UtilsStorageWriter : public rti::recording::storage::StorageWriter {
public:
    typedef std::map<std::string, std::ofstream> OutputFileSet;
    typedef OutputFileSet::value_type FileSetEntry;

    explicit UtilsStorageWriter(const rti::routing::PropertySet& properties);
    virtual ~UtilsStorageWriter();

    /**
     * @brief Returns a UtilsStorageProperty reference with the default values
     * of the plug-in.
     */
    static const UtilsStorageProperty& PROPERTY_DEFAULT();

    /**
     * @brief Returns the property namespace shared across all the different
     * plug-in configuration properties.
     *
     * Value: rti.recording.utils_storage
     */
    static const std::string& PROPERTY_NAMESPACE();

    /**
     * @brief Returns the name of the property that configures
     * UtilsStorageProperty::output_dir_path.
     *
     * Value: [namespace].output_dir_name
     */
    static const std::string& OUTPUT_DIR_PROPERTY_NAME();

    /**
     * @brief Returns the name of the property that configures
     * UtilsStorageProperty::output_file_base_name
     *
     * Value: [namespace].output_file_basename
     */
    static const std::string& OUTPUT_FILE_BASENAME_PROPERTY_NAME();


    static const std::string& OUTPUT_FORMAT_PROPERTY_NAME();

    /**
     * @brief Returns the name of the property that configures
     * UtilsStorageProperty::merge_output_files
     *
     * Value: [namespace].merge_output_files
     */
    static const std::string& OUTPUT_MERGE_PROPERTY_NAME();

    /**
     * @brief Returns the name of the property that configures
     * the verbosity level for the logging messages.
     *
     * Value: [namespace].verbosity
     */
    static const std::string& LOGGING_VERBOSITY_PROPERTY_NAME();

    /**
     * @brief Returns the name of the property that configures
     * PrintFormatCsvProperty::empty_member_value_representation
     *
     * Value: [namespace].csv.empty_member_value
     */
    static const std::string& CSV_EMPTY_MEMBER_VALUE_REP_PROPERTY_NAME();

    /**
     * @brief Returns the name of the property that configures
     * PrintFormatCsvProperty::enum_as_string
     *
     * Value: [namespace].csv.enum_as_string
     */
    static const std::string& CSV_ENUM_AS_STRING_PROPERTY_NAME();

    /**
     * @brief Returns the file extension for the files that contain the data
     * in CSV format.
     *
     * Value: .csv
     */
    static const std::string& CSV_FILE_EXTENSION();

    /**
     * @brief Returns the default value of the prefix used for the output file
     * names.
     *
     * Value: csv_converted
     */
    static const std::string& OUTPUT_FILE_BASENAME_DEFAULT();

    /* --- StorateWriter implementation ------------------------------------ */

    /**
     * @brief Creates a UtilsStreamWriter for each stream/topic.
     *
     * @see UtilsStreamWriter
     */
    rti::recording::storage::StorageStreamWriter * create_stream_writer(
            const rti::routing::StreamInfo& stream_info,
            const rti::routing::PropertySet&) override;

    /**
     * @brief Deletes the UtilsStreamWriter.
     */
    void delete_stream_writer(
            rti::recording::storage::StorageStreamWriter *writer) override;

private:
    void merge_output_file(FileSetEntry& file_entry);

    UtilsStorageProperty property_;
    // Collection of output files, one for each stream
    OutputFileSet output_files_;
    // The final file if merging is enabled
    std::ofstream output_merged_file_;
    // Property per output kind
    PrintFormatCsvProperty csv_property_;
};

/**
 * @brief Base abstract class for all the StreamWriter implementations part
 * of the UtilsStorageWriter.
 *
 * This interface extends the behavior of DynamicDataStorageStreamWriter to
 * also retrieve the UtilsStorageWriter::FileSetEntry that the StreamWriter
 * is using to store the data.
 *
 * Implementations of this class also implement DynamicDataStorageStreamWriter
 * in order to write samples into an output text file with a specified format.
 * Each implementation is responsible to handle a specific format.
 *
 * Known implementations:
 * - CsvStreamWriter
 *
 */
class UtilsStreamWriter :
        public rti::recording::storage::DynamicDataStorageStreamWriter {
public:

    virtual UtilsStorageWriter::FileSetEntry& file_entry() = 0;
};

/**
 * @brief Implementation of a UtilsStreamWriterr that writes samples
 * into an output text file with an specified format.
 *
 * Currently, the implementation only supports CSV format.
 */
class CsvStreamWriter : public UtilsStreamWriter {
public:

    /**
     * @brief Creates an UtilsStreamWriter responsible for storing the
     * data in a file in CSV format.
     *
     * @param[in] property CSV output configuration elements.
     * @param[in] stream_info Information associated to the stream/topic
     * @param[in] output_file_entry The output file where data is pushed.
     */
    CsvStreamWriter(
            const PrintFormatCsvProperty& property,
            const rti::routing::StreamInfo& stream_info,
            UtilsStorageWriter::FileSetEntry& output_file_entry);


    virtual ~CsvStreamWriter() override;

    /**
     * @brief Writes the input samples into a CSV file. Each sample is placed
     * in a separate row.
     *
     * @override Implementation of DynamicDataStorageStreamWriter::store
     */
    void store(
            const std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
            const std::vector<dds::sub::SampleInfo *>& info_seq) override;

    /**
     * @brief Returns the file entry used by this UtilsStorageWriter to write
     * samples.
     *
     * @override UtilsStreamWriter::file_entry
     */
    UtilsStorageWriter::FileSetEntry& file_entry() override;

private:
    // PrintFormat implementation used to convert data samples
    PrintFormatCsv print_format_csv_;
    UtilsStorageWriter::FileSetEntry& output_file_entry_;
    // a buffer to represent a single CSV sample
    std::string data_as_csv_;
};

} } }

#endif

