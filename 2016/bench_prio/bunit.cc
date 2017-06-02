/**
 * Non-metric Space Library
 *
 * Authors: Bilegsaikhan Naidan (https://github.com/bileg), Leonid Boytsov (http://boytsov.info).
 * With contributions from Lawrence Cayton (http://lcayton.com/) and others.
 *
 * For the complete list of contributors and further details see:
 * https://github.com/searchivarius/NonMetricSpaceLib 
 * 
 * Copyright (c) 2014
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 */

#include <stdlib.h>

#include <string>

#include "bunit.h"

#ifdef __linux
const std::string green = "\x1b[32m";
const std::string red = "\x1b[31m";
const std::string yellow = "\x1b[33m";
const std::string no_color = "\x1b[0m";
#else
const std::string green;
const std::string red;
const std::string yellow;
const std::string no_color;
#endif

TestRunner& TestRunner::Instance() {
  static TestRunner instance;
  return instance;
}

void TestRunner::AddTest(const std::string& test_name, TestBase* test_instance) {
  const size_t pos = test_name.find(std::string(kDisable));
  bool disabled = false;
  if (pos != std::string::npos) {
    if (pos == 0) {
      disabled = true;
    } else {
      std::cout << "Incorrect test name " << test_name << std::endl;
      exit(1);
    }
  }
  tests_.push_back(std::make_tuple(test_name, test_instance, disabled));
}

int TestRunner::RunAllTests() {
  int num_failed = 0;
  int num_disabled = 0;
  for (auto it = tests_.begin(); it != tests_.end(); ++it) {
    std::cout << "----- " << std::get<0>(*it) << " -----" << std::endl;
    if (std::get<2>(*it)) {
      ++num_disabled;
      std::cout << yellow << "disabled" << no_color << std::endl;
    } else {
      try {
        std::get<1>(*it)->Test();
        std::cout << green << "passed" << no_color << std::endl;
      } catch (const TestException& ex) {
          ++num_failed;
          std::cout << red << "failed" << no_color << std::endl;
          std::cout << ex.what() << std::endl;
      } catch (const std::exception& ex) {
        ++num_failed;
        std::cout << red << "failed" << no_color << std::endl;
        std::cout << ex.what() << std::endl;
      } catch (...) {
        std::cout << red << "failed" << no_color << std::endl;
        std::cout << "unknown cause" << std::endl;
      }
    }
  }

  std::cout << "======================================" << std::endl;
  std::cout << green << "In total " << tests_.size() << " testcases";
  if (num_disabled > 0) {
    std::cout << " (" << num_disabled << " tests disabled)";
  }
  std::cout << no_color << std::endl;

  if (num_failed == 0) {
    std::cout << green << "ALL TESTS PASSED" << no_color << std::endl;
  } else {
    std::cout << red << "FAILED " << num_failed
              << " TESTS !!!" << no_color << std::endl;
  }
  std::cout << "======================================" << std::endl;

  return num_failed;
}

int main(int argc, char *argv[]) {
  return TestRunner::Instance().RunAllTests();
}
