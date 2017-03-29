#include <fstream>
#include <iostream>
#include <iterator>
#include <sys/file.h>
#include <unistd.h>

#include "cgrapher.hpp"

namespace
{
    FunctionsCollectorConsumer::FunctionsCollectorConsumer(clang::CompilerInstance & __CI, const std::string & root, const std::string & lock, const std::string & out) : clang::ASTConsumer(), CI(__CI), visitor(__CI, root, lock, out) { }
    
    FunctionsCollectorConsumer::~FunctionsCollectorConsumer() { }

    void FunctionsCollectorConsumer::HandleTranslationUnit(clang::ASTContext & ctxt)
    {
        visitor.TraverseDecl(ctxt.getTranslationUnitDecl());
        visitor.dump();
    }

    bool FunctionsCollectorConsumer::shouldSkipFunctionBody(clang::Decl * decl)
    {
        return false;
    }

    std::unique_ptr<clang::ASTConsumer> FunctionsCollectorAction::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef)
    {
        return llvm::make_unique<FunctionsCollectorConsumer>(CI, root, lock, out);
    }

    bool FunctionsCollectorAction::ParseArgs(const clang::CompilerInstance & CI, const std::vector<std::string> & args)
    {
        if (args.size() >= 1)
        {
            root = args[0];
            if (args.size() >= 2)
            {
                out = args[1];
                if (args.size() >= 3)
                {
                    lock = args[2];
                }
            }
        }
        
        return true;
    }

        // Automatically run the plugin after the main AST action
        clang::PluginASTAction::ActionType FunctionsCollectorAction::getActionType()
        {
            return AddAfterMainAction;
        }
        
        static clang::FrontendPluginRegistry::Add<FunctionsCollectorAction> X("cgrapher", "get a call graph");
}

