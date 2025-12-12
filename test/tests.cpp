#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "parser.h"

bool isValidProgramFile(const std::string& path, std::string& errorMsg) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    Program* prog = nullptr;
    try {
        prog = parseProgram(ss.str());
        if (!prog) {
            delete prog;
            return false;
        }
        delete prog;
        return true;
    } catch (const std::exception& ex) {
        errorMsg = ex.what();
        delete prog;
        return false;
    }
}

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cout << "Test failed: " << #cond << " with error " << errorMsg << "\n"; \
            return false; \
        }  else { \
            std::cout << "Test passed: " << #cond << "\n"; \
        } \
    } while (0)

#define ASSERT_FALSE(cond) \
    do { \
        if (cond) { \
            std::cout << "Test failed: !" << #cond << "\n"; \
            return false; \
        } else { \
            std::cout << "Test passed: !" << #cond << "\n"; \
        } \
    } while (0)


bool runTests() {
    std::string errorMsg;
    ASSERT_TRUE(isValidProgramFile("test/examples/01_midterm_clamp.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/02_homework_final.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/03_drop_lowest.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/04_pass_fail.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/05_resolve_undef.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/01_midterm_clamp_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/02_homework_final_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/03_drop_lowest_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/04_pass_fail_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/05_resolve_undef_bad.txt", errorMsg));
    // added passing examples
    ASSERT_TRUE(isValidProgramFile("test/examples/06_simple_int.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/07_simple_double.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/08_simple_percent.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/09_ref_to_other.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/10_op_sum.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/11_op_weighted.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/12_list_basic.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/13_list_weighted.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/14_nested_list.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/15_op_nested.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/16_two_assignments.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/17_percent_and_double.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/18_complex_op.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/19_op_with_refs.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/20_list_with_ops.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/21_special_chars_name.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/22_empty_op.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/23_integer_list_weights.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/24_nested_operations.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/25_big_float.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/26_int_and_percent.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/27_id_with_dots.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/28_category_mix.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/29_deep_nest.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/30_list_of_lists.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/31_op_and_list.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/32_zero_percent.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/33_multiple_ids.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/34_operation_many_args.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/35_mixed_types.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/36_weight_with_expr.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/37_paren_chain.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/38_single_list_item.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/39_nested_mixed.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/40_ref_simple.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/41_trailing_comments.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/42_multiple_assignments.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/43_op_with_percent.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/44_zero_double.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/45_long_list.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/46_op_no_space.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/47_mixed_ops.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/48_complex_list.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/49_many_slash_id.txt", errorMsg));
    ASSERT_TRUE(isValidProgramFile("test/examples/50_hyphen_slash_ok.txt", errorMsg));
    // added failing examples
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/06_no_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/07_unmatched_brace.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/08_unmatched_paren.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/09_invalid_char.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/10_missing_expr_after_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/11_op_missing_close.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/12_list_missing_rbrace.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/13_colon_in_wrong_place.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/14_bare_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/15_unknown_token.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/16_unterminated_comment.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/17_list_colon_no_weight.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/18_op_colon_confuse.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/19_just_paren.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/20_closing_brace_without_open.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/21_leading_colon_with_number.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/22_percent_bad.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/23_dot_standalone.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/24_list_nested_unbalanced.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/25_name_with_space.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/26_colon_after_number.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/27_bad_op_syntax.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/28_only_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/29_weight_missing_expr.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/30_actual_math.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/31_bad_percent_format.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/32_paren_no_ident.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/33_double_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/34_misplaced_rparen.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/35_empty_braces_percent.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/36_trailing_colon_line.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/37_invalid_percent_chars.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/38_name_ends_with_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/39_unterminated_multiline_comment.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/40_paren_mismatch_deep.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/41_colon_only_in_file.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/42_bad_char_in_number.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/43_brace_then_paren.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/44_weird_token.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/45_nested_ops_missing_close.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/46_leading_colon.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/47_just_comment_chars.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/48_weird_sequence.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/49_invalid_identifier_chars.txt", errorMsg));
    ASSERT_FALSE(isValidProgramFile("test/examples/bad/50_only_open_brace.txt", errorMsg));
    std::cout << "All tests passed." << std::endl; // make sure to flush output
    return true;
}

int main() {
    if (!runTests()) {
        return 1;
    }
    return 0;
}
