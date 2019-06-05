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

#ifndef RTI_RECORDER_UTILS_PRINTFORMATCSV_HPP_
#define RTI_RECORDER_UTILS_PRINTFORMATCSV_HPP_

#include <fstream>
#include <iostream>
#include <stack>

#include "dds_c/dds_c_printformat.h"
#include "dds/core/xtypes/DynamicType.hpp"
#include "dds/core/xtypes/MemberType.hpp"

#include "PrintFormatWrapper.hpp"

namespace rti { namespace recorder { namespace utils {

/**
 * @brief Configuration elements of the CSV output format
 *
 */
class PrintFormatCsvProperty {
public:
    PrintFormatCsvProperty();

    /**
     * @brief Specifies the value to be used as a data entry for a cell
     * corresponding to an empty member, such as a missing sequence element
     * or optional members.
     *
     * Default: [empty]
     */
    PrintFormatCsvProperty& empty_member_value_representation(const std::string& value);

    /**
     *
     * @brief Gets  the empty_member_value_representation
     */
    const std::string& empty_member_value_representation() const;

    /**
     * @brief Indicates whether enumeration members are represented with
     * their label string representation or numeric value.
     *
     * Default: false
     */
    PrintFormatCsvProperty& enum_as_string(bool the_enum_as_string);

    /**
     * @brief Gets the enum_as_string
     */
    bool enum_as_string() const;


private:
    std::string empty_member_value_rep_;
    bool enum_as_string_;
    
};

class NativePrintFormatCsv;

/**
 * @brief Wrapper implementation of DDS_PrintFormat to convert a DynamicData
 * sample into its equivalent CSV representation.
 *
 * The implementation relies on the construction of a tree of ColumnInfo objects.
 * A ColumnInfo object is created for each member of the type. If the member
 * is a complex or collection type, the ColumnInfo will contain a list of
 * children representing the contained elements. This process applies
 * recursively. The ColumnInfo tree is constructed on PrintFormatCsv creation
 * from the specified data type.
 *
 * For each data sample, a PrintFormatCsv will dynamically construct a stack
 * of Cursors, where a Cursor points to a specify ColumnEntry. This allows
 * to a PrintFormatCsv object to maintain a correspondence between the current
 * member being printed (notified by one of the DDS_PrintFormat callbacks) and
 * its associated ColumnInfo.
 *
 * This is needed specially for the situation where a bunch of member elements 
 * are skipped because they are not present in a given DynamicData sample.
 * Since CSV requires consistent columns, even if a member is not present an
 * "empty" column with a separator needs to be placed. The Cursor allows to
 * determine how many columns need to be skipped by simply moving the Cursor
 * until it reaches the end of siblings from the ColumnInfo the current Cursor
 * points to.
 *
 * Skipping members may occur for the following types:
 * - Optional members
 * - Sequences
 * - Unions
 *
 * @see DDS_PrintFormat
 */
class PrintFormatCsv {
public:

    /**
     * @brief Definition of the information associated with a data column.
     *
     */
    class ColumnInfo {
    public:
        typedef std::vector<ColumnInfo>::const_iterator iterator;

        /**
         * @brief Default constructor required to be used with C++ collections.
         */
        ColumnInfo();
        
        /**
         * @brief Creates a ColumnInfo for a member
         *
         * @param[in] name Name of the member
         * @param[in] type Type of the member
         */
        ColumnInfo(
                const std::string& name,
                const dds::core::xtypes::DynamicType& type);

        /**
         * @brief Adds the specified info as child of this info.
         */
        ColumnInfo& add_child(const ColumnInfo&& info);

        /**
         * @brief Returns the parent of this info/
         */
        const ColumnInfo& parent() const;

        /**
         * @brief Returns iterator to the first child.
         */
        iterator first_child() const;

        /**
         * @brief returns a reference to the list of children of this info
         */
        const std::vector<ColumnInfo>& children() const;

        /**
         * @brief Returns he type kind of the member this info represents
         */
        const dds::core::xtypes::TypeKind type_kind() const;

        /**
         * @brief Sets the type kind of the member this info represents
         */
        ColumnInfo& type_kind(const dds::core::xtypes::TypeKind type_kind);

        /**
         * @brief Returns the name of the member this info represents
         */
        const std::string& name() const;

        /**
         * @brief Returns whether this info has a parent or not
         */
        bool has_parent() const;

        /**         
         * @brief Returns whether the member this info represents is of a
         * collection type.
         */
        bool is_collection() const;

        /**
         * @brief String representation of a ColumnInfo
         */
        friend std::ostream& operator<<(
                std::ostream& os,
                const ColumnInfo& info);
        
    private:
        const ColumnInfo *parent_;
        std::string name_;
        dds::core::xtypes::TypeKind type_kind_;
        std::vector<ColumnInfo> children_;
    };
    
public:
    typedef ColumnInfo::iterator Cursor;
    typedef std::vector<Cursor> CursorStack;

    /**
     * @brief Returns the default value of the column separator used in the
     * output CSV file.
     *
     * Value:  \p ","
     */
    static const std::string& COLUMN_SEPARATOR_DEFAULT();

    /**
     * @brief Returns the default value used to represent the empty member cell.
     *
     * Value:  \p "nil"
     */
    static const std::string& EMPTY_MEMBER_VALUE_REPRESENTATION_DEFAULT();
    
    /**
     * @brief Returns a PrintFormatCsvProperty reference with the default values
     * of this implementation.
     */
    static const PrintFormatCsvProperty& PROPERTY_DEFAULT();

    /**
     * @brief Constructor
     * 
     * @param[in] property Configuration elements
     * @param[in] type Type of the samples to be converted
     * @param[in] output_file Output file where the converted samples are written.
     */
    PrintFormatCsv(
            const PrintFormatCsvProperty& property,
            const dds::core::xtypes::DynamicType& type,
            std::ofstream& output_file);

    /*
     * @brief Returns this object's configuration property
     */
    const PrintFormatCsvProperty& property() const;

    /**
     *  @brief Returns the native implementation of DDS_PrintFormat
     */
    DDS_PrintFormat* native();

    /**
     * @brief Returns the PrintFormatCsv wrapper object from the native
     * representation.
     */
    static PrintFormatCsv& from_native(DDS_PrintFormat *native);

    /**
     * @brief Returns the file stream handle to the output file where samples
     * are placed.
     */
    std::ofstream& output_file();    

private:
    friend class NativePrintFormatCsv;

    /**
     * @brief Resets the state of this object to start printing a new data
     * sample.
     */
    void start_data_conversion();

    /**
     * @brief Returns a reference to the current Cursor, which points to the
     * current member being printed. This corresponds with the top element
     * of the Cursor stack.
     */
    Cursor& cursor();

    /**
     * @brief Returns a copy of the parent Cursor of the current one.
     */
    PrintFormatCsv::Cursor parent_cursor();

    /**
     * @brief Push the next cursor in the stack. The new top Cursor points
     * to the first child of the current Cursor.
     */
    void push_cursor();

    /**
     * @brief Pop the current top Cursor in the stack to the new top points
     * to the parent Cursor.
     */
    void pop_cursor();    

    /**
     * @brief Skips all the remaining siblings of the current Cursor (stack top)
     * 
     * @see skip_cursor
     */
    void skip_cursor_siblings(RTIXMLSaveContext* save_context);

    /**
     * @brief Skip the member pointed to by the current cursor (stack top) by
     * inserting an empty value representation and a separator.
     *     
     * The value used for the skip member is given by
     * PrintFormatCsvProperty::empty_member_value_representation.     
     *    
     * @param[in] save_context The output save context where the skip columns
     *                         are written.
     */
    void skip_cursor(RTIXMLSaveContext* save_context);

    /**
     * @brief Skips the current cursor to point to the specified member.
     *
     * @see skip_cursor
     * 
     * @param[in] member_name   Name of the member to position the cursor to.
     * @param[in] save_context The output save context where the skip columns
     *                         are written.
     */
    void skip_cursor(
            const std::string& member_name,
            RTIXMLSaveContext *save_context);

    /**
     * @brief Generates the ColumnInfo tree for the type associated with
     * this object.
     * 
     * @param current_info Reference to the info associated with a member
     * @param member_type Type of the member represented by current_info.
     */
    void build_column_info(
            ColumnInfo& current_info,
            const dds::core::xtypes::DynamicType& member_type);

    /**
     *
     * @brief Generates the type header in CSV format.
     *
     * The type header is a row with as many columns as possible members of
     * a data sample can have. See manual for details on the format of the
     * type header.
     *
     * @param[in] current_info a top-level element of the ColumnInfo tree
     * @param[out[ string_stream    The output stream where the type header
     *                              is generated.
     */
    void print_type_header(
            std::ostringstream& string_stream,
            const ColumnInfo& current_info);

    /**
     * @brief Skip all the siblings of the specified cursor by
     * inserting an empty value representation and a separator.
     *
     * @param[in] cursor    The cursor
     * @param[in] save_context The output save context where the skip columns
     *                         are written.
     * @param[in] empty_member_value_rep  Value to write in the empty member cell
     */
    static void skip_cursor_columns(
            PrintFormatCsv::Cursor& cursor,
            RTIXMLSaveContext* save_context,
            const std::string& empty_member_value_rep);

    void initialize_native();    

private:
    PrintFormatWrapper<PrintFormatCsv> format_wrapper_;
    const PrintFormatCsvProperty& property_;
    dds::core::xtypes::DynamicType type_;
    std::ofstream& output_file_;
    ColumnInfo column_info_;
    CursorStack cursor_stack_;
};

} } }

#endif
