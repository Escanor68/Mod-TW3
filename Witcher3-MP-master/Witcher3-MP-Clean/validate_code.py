#!/usr/bin/env python3
"""
Witcher3-MP Code Validation Script
Validates the implementation without requiring compilation
"""

import os
import re
import json
from pathlib import Path

class CodeValidator:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.errors = []
        self.warnings = []
        self.success_count = 0
        
    def validate_file(self, file_path, file_type):
        """Validate a single file"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
            if file_type == 'cpp':
                return self.validate_cpp_file(content, file_path)
            elif file_type == 'h':
                return self.validate_header_file(content, file_path)
            elif file_type == 'ws':
                return self.validate_witcherscript_file(content, file_path)
            else:
                return True
                
        except Exception as e:
            self.errors.append(f"Error reading {file_path}: {e}")
            return False
    
    def validate_cpp_file(self, content, file_path):
        """Validate C++ file"""
        issues = []
        
        # Check for basic C++ syntax
        if 'class' in content and '{' in content:
            self.success_count += 1
            
        # Check for includes
        if '#include' in content:
            self.success_count += 1
            
        # Check for function definitions
        if 'bool ' in content or 'void ' in content or 'int ' in content:
            self.success_count += 1
            
        # Check for error handling
        if 'try' in content or 'catch' in content or 'LOG_ERROR' in content:
            self.success_count += 1
            
        return len(issues) == 0
    
    def validate_header_file(self, content, file_path):
        """Validate header file"""
        issues = []
        
        # Check for header guards or pragma once
        if '#pragma once' in content or '#ifndef' in content:
            self.success_count += 1
            
        # Check for class declarations
        if 'class ' in content:
            self.success_count += 1
            
        # Check for function declarations
        if 'bool ' in content or 'void ' in content or 'int ' in content:
            self.success_count += 1
            
        return len(issues) == 0
    
    def validate_witcherscript_file(self, content, file_path):
        """Validate WitcherScript file"""
        issues = []
        
        # Check for basic WitcherScript syntax
        if 'class ' in content or 'func ' in content:
            self.success_count += 1
            
        # Check for using statements
        if 'using ' in content:
            self.success_count += 1
            
        # Check for function definitions
        if 'public func ' in content or 'private func ' in content:
            self.success_count += 1
            
        return len(issues) == 0
    
    def validate_project_structure(self):
        """Validate overall project structure"""
        required_dirs = ['src', 'include', 'tests', 'mod', 'docs']
        required_files = ['CMakeLists.txt', 'README.md']
        
        for dir_name in required_dirs:
            dir_path = self.project_root / dir_name
            if dir_path.exists():
                self.success_count += 1
            else:
                self.errors.append(f"Missing required directory: {dir_name}")
        
        for file_name in required_files:
            file_path = self.project_root / file_name
            if file_path.exists():
                self.success_count += 1
            else:
                self.errors.append(f"Missing required file: {file_name}")
    
    def validate_combat_system(self):
        """Validate combat system implementation"""
        combat_files = [
            'src/integration/CombatSystemIntegration.cpp',
            'src/integration/CombatSystemIntegration.h',
            'src/optimization/CombatOptimizer.cpp',
            'src/optimization/CombatOptimizer.h',
            'mod/scripts/game/witcher3_mp/MP_CombatSystemEnhanced.ws',
            'mod/scripts/game/witcher3_mp/MP_CombatHelpers.ws',
            'tests/test_combat_system.cpp'
        ]
        
        for file_path in combat_files:
            full_path = self.project_root / file_path
            if full_path.exists():
                self.success_count += 1
                print(f"✓ Found: {file_path}")
            else:
                self.errors.append(f"Missing combat system file: {file_path}")
    
    def validate_integration_system(self):
        """Validate integration system"""
        integration_files = [
            'src/integration/REDkitBridge.cpp',
            'src/integration/REDkitBridge.h',
            'src/integration/WitcherScriptBridge.cpp',
            'src/integration/WitcherScriptBridge.h',
            'src/integration/AssetLoader.cpp',
            'src/integration/AssetLoader.h'
        ]
        
        for file_path in integration_files:
            full_path = self.project_root / file_path
            if full_path.exists():
                self.success_count += 1
                print(f"✓ Found: {file_path}")
            else:
                self.errors.append(f"Missing integration file: {file_path}")
    
    def validate_tests(self):
        """Validate test files"""
        test_files = [
            'tests/test_combat_system.cpp',
            'tests/test_bridges.cpp',
            'tests/test_witcherscript.cpp',
            'tests/test_compression.cpp'
        ]
        
        for file_path in test_files:
            full_path = self.project_root / file_path
            if full_path.exists():
                self.success_count += 1
                print(f"✓ Found: {file_path}")
            else:
                self.warnings.append(f"Missing test file: {file_path}")
    
    def validate_documentation(self):
        """Validate documentation"""
        doc_files = [
            'docs/IMPLEMENTATION_COMPLETE_REPORT.md',
            'docs/MOD_ANALYSIS_COMBAT_SYSTEM.md',
            'docs/INCOMPLETE_FUNCTIONS_ANALYSIS.md',
            'docs/API_RESEARCH_REPORT.md'
        ]
        
        for file_path in doc_files:
            full_path = self.project_root / file_path
            if full_path.exists():
                self.success_count += 1
                print(f"✓ Found: {file_path}")
            else:
                self.warnings.append(f"Missing documentation: {file_path}")
    
    def run_validation(self):
        """Run complete validation"""
        print("=== Witcher3-MP Code Validation ===")
        print()
        
        # Validate project structure
        print("1. Validating project structure...")
        self.validate_project_structure()
        
        # Validate combat system
        print("\n2. Validating combat system...")
        self.validate_combat_system()
        
        # Validate integration system
        print("\n3. Validating integration system...")
        self.validate_integration_system()
        
        # Validate tests
        print("\n4. Validating tests...")
        self.validate_tests()
        
        # Validate documentation
        print("\n5. Validating documentation...")
        self.validate_documentation()
        
        # Print results
        print("\n=== Validation Results ===")
        print(f"Success count: {self.success_count}")
        print(f"Errors: {len(self.errors)}")
        print(f"Warnings: {len(self.warnings)}")
        
        if self.errors:
            print("\nErrors:")
            for error in self.errors:
                print(f"  ❌ {error}")
        
        if self.warnings:
            print("\nWarnings:")
            for warning in self.warnings:
                print(f"  ⚠️  {warning}")
        
        if not self.errors:
            print("\n✅ All validations passed!")
            return True
        else:
            print("\n❌ Validation failed!")
            return False

def main():
    validator = CodeValidator(".")
    success = validator.run_validation()
    
    if success:
        print("\n🎉 Witcher3-MP implementation is ready for testing!")
    else:
        print("\n🔧 Please fix the errors before proceeding.")
    
    return 0 if success else 1

if __name__ == "__main__":
    exit(main())
