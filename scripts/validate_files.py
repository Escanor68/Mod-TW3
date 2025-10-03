#!/usr/bin/env python3
"""
Witcher3-MP File Validation Script
Validates JSON, XML, and WitcherScript files for syntax errors
"""

import json
import xml.etree.ElementTree as ET
import os
import sys
import re
from pathlib import Path

class FileValidator:
    def __init__(self, log_file="logs/validation.log"):
        self.log_file = log_file
        self.errors = []
        self.warnings = []
        self.validated_files = 0
        
        # Create logs directory if it doesn't exist
        os.makedirs(os.path.dirname(log_file), exist_ok=True)
        
        # Initialize log file
        with open(log_file, 'w', encoding='utf-8') as f:
            f.write(f"=== Witcher3-MP File Validation Log ===\n")
            f.write(f"Validation started at: {self.get_timestamp()}\n\n")
    
    def get_timestamp(self):
        from datetime import datetime
        return datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    def log(self, message, level="INFO"):
        timestamp = self.get_timestamp()
        log_message = f"[{timestamp}] [{level}] {message}"
        print(log_message)
        
        with open(self.log_file, 'a', encoding='utf-8') as f:
            f.write(log_message + "\n")
    
    def validate_json(self, file_path):
        """Validate JSON file syntax"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                json.load(f)
            self.log(f"✅ {file_path} - Valid JSON")
            return True
        except json.JSONDecodeError as e:
            error_msg = f"❌ {file_path} - Invalid JSON: {str(e)}"
            self.log(error_msg, "ERROR")
            self.errors.append(error_msg)
            return False
        except Exception as e:
            error_msg = f"❌ {file_path} - Error reading file: {str(e)}"
            self.log(error_msg, "ERROR")
            self.errors.append(error_msg)
            return False
    
    def validate_xml(self, file_path):
        """Validate XML file syntax"""
        try:
            ET.parse(file_path)
            self.log(f"✅ {file_path} - Valid XML")
            return True
        except ET.ParseError as e:
            error_msg = f"❌ {file_path} - Invalid XML: {str(e)}"
            self.log(error_msg, "ERROR")
            self.errors.append(error_msg)
            return False
        except Exception as e:
            error_msg = f"❌ {file_path} - Error reading file: {str(e)}"
            self.log(error_msg, "ERROR")
            self.errors.append(error_msg)
            return False
    
    def validate_witcherscript(self, file_path):
        """Validate WitcherScript file syntax (basic checks)"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Basic syntax checks for WitcherScript
            issues = []
            
            # Check for balanced braces
            open_braces = content.count('{')
            close_braces = content.count('}')
            if open_braces != close_braces:
                issues.append(f"Unbalanced braces: {open_braces} open, {close_braces} close")
            
            # Check for balanced parentheses
            open_parens = content.count('(')
            close_parens = content.count(')')
            if open_parens != close_parens:
                issues.append(f"Unbalanced parentheses: {open_parens} open, {close_parens} close")
            
            # Check for common syntax issues
            if 'function ' in content and 'func ' in content:
                issues.append("Mixed function declaration syntax (use 'func' consistently)")
            
            # Check for missing semicolons in common patterns
            lines = content.split('\n')
            for i, line in enumerate(lines, 1):
                line = line.strip()
                if (line.startswith('let ') or line.startswith('var ')) and not line.endswith(';') and not line.endswith('{'):
                    if not any(keyword in line for keyword in ['if', 'while', 'for', 'switch']):
                        issues.append(f"Line {i}: Missing semicolon after variable declaration")
            
            if issues:
                warning_msg = f"⚠️  {file_path} - Potential issues: {'; '.join(issues)}"
                self.log(warning_msg, "WARNING")
                self.warnings.append(warning_msg)
                return False
            else:
                self.log(f"✅ {file_path} - Valid WitcherScript")
                return True
                
        except Exception as e:
            error_msg = f"❌ {file_path} - Error reading file: {str(e)}"
            self.log(error_msg, "ERROR")
            self.errors.append(error_msg)
            return False
    
    def validate_directory(self, directory, file_patterns):
        """Validate all files matching patterns in directory"""
        if not os.path.exists(directory):
            self.log(f"❌ Directory not found: {directory}", "ERROR")
            return False
        
        self.log(f"🔍 Validating files in: {directory}")
        
        for pattern in file_patterns:
            for file_path in Path(directory).rglob(pattern):
                self.validated_files += 1
                
                if pattern == "*.json":
                    self.validate_json(str(file_path))
                elif pattern == "*.xml":
                    self.validate_xml(str(file_path))
                elif pattern == "*.ws":
                    self.validate_witcherscript(str(file_path))
    
    def generate_summary(self):
        """Generate validation summary"""
        self.log("\n" + "="*50)
        self.log("VALIDATION SUMMARY")
        self.log("="*50)
        self.log(f"Files validated: {self.validated_files}")
        self.log(f"Errors found: {len(self.errors)}")
        self.log(f"Warnings found: {len(self.warnings)}")
        
        if self.errors:
            self.log("\nERRORS:")
            for error in self.errors:
                self.log(f"  {error}")
        
        if self.warnings:
            self.log("\nWARNINGS:")
            for warning in self.warnings:
                self.log(f"  {warning}")
        
        if not self.errors and not self.warnings:
            self.log("\n🎉 ALL FILES VALIDATED SUCCESSFULLY!")
            return True
        elif not self.errors:
            self.log("\n✅ VALIDATION COMPLETED WITH WARNINGS")
            return True
        else:
            self.log("\n❌ VALIDATION FAILED WITH ERRORS")
            return False

def main():
    print("🔍 Witcher3-MP File Validation")
    print("="*40)
    
    validator = FileValidator()
    
    # Validate configuration files
    validator.log("Validating configuration files...")
    validator.validate_directory("mod/config", ["*.json", "*.xml"])
    validator.validate_directory("config", ["*.json"])
    
    # Validate WitcherScript files
    validator.log("Validating WitcherScript files...")
    validator.validate_directory("mod/scripts", ["*.ws"])
    
    # Generate summary
    success = validator.generate_summary()
    
    # Write results to test results file
    results_file = "logs/test_results.log"
    with open(results_file, 'a', encoding='utf-8') as f:
        f.write(f"\nFile Validation Results:\n")
        f.write(f"  Files validated: {validator.validated_files}\n")
        f.write(f"  Errors: {len(validator.errors)}\n")
        f.write(f"  Warnings: {len(validator.warnings)}\n")
        f.write(f"  Status: {'PASSED' if success else 'FAILED'}\n")
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
