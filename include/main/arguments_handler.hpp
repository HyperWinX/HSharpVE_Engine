#pragma once

#include <memory>
#include <vector>
#include <ostream>
#include <variant>
#include <cstdint>

#include <version.hpp>
#include <argparse/argparse.hpp>


namespace hsharp {

    // Directives are actions, performed by building
    // tool which are not part of compilation job.
    // Simplest example is help and version printing actions.
    enum class EDirective : std::int_fast8_t {
        HELP, VERSION
    };

    std::string mapDirective(EDirective directive, bool useCLINotation = false);

    void displayHelp(std::ostream& os);
    void displayVersion(std::ostream& os);

    // Supported directives by parser and their precedence
    inline const std::vector<EDirective> supportedDirectives {
        EDirective::HELP, 
        EDirective::VERSION
    };

    using ArgumentType = std::variant<std::string, EDirective, bool>;

    // CLIRegistry keeps track of all flags, variables and directives
    // passed to compiler.
    class CLIRegistry {
    public:

        // IArgumentsAccessor is used as a generic interface
        // to accessing CLI arguments.
        class IArgumentsAccessor {
        public:
            virtual void set(const std::string& argument, ArgumentType value) = 0;
            virtual const ArgumentType& get(const std::string& argument) const = 0;
            virtual bool has(const std::string& argument) const = 0;
            virtual const ArgumentType& operator[](const std::string& argument) const = 0;
            virtual ~IArgumentsAccessor() = default;
        };

        // Constructors
        CLIRegistry(std::unique_ptr<const IArgumentsAccessor> accessor);
        CLIRegistry(CLIRegistry&&) = default;
        CLIRegistry(const CLIRegistry&) = delete;

        const std::vector<EDirective>& getDirectives() const noexcept;
        const std::string& getFilename() const noexcept;

    private:
        void handleRegistrySetup(std::unique_ptr<const IArgumentsAccessor> accessor);

    private:
        std::vector<EDirective> directives_;
        std::string filename_;
    };

    // Main Handler.
    class ArgumentsHandler {
    public:

        // Simple In-and-Out accessor.
        class SIOArgumentsAccessor : public CLIRegistry::IArgumentsAccessor {
        public:
            virtual void set(const std::string& argument, ArgumentType value) override;
            virtual const ArgumentType& get(const std::string& argument) const override;
            virtual bool has(const std::string& argument) const override;
            virtual const ArgumentType& operator[](const std::string& argument) const override;

        private:
            std::map<std::string, ArgumentType> args_;
        };

        // Constructors
        ArgumentsHandler() = default;
        ArgumentsHandler(const ArgumentsHandler&) = delete;
        ArgumentsHandler(ArgumentsHandler&&) = default;

        void parse(std::unique_ptr<CLIRegistry::IArgumentsAccessor> accessor, int argc, char *argv[]);
        void clear();

        const CLIRegistry& registry() const;

    private:
        void handleDirectives(argparse::ArgumentParser& parser, std::unique_ptr<CLIRegistry::IArgumentsAccessor>& accessor) noexcept;
        void handleVariables(argparse::ArgumentParser& parser, std::unique_ptr<CLIRegistry::IArgumentsAccessor>& accessor) noexcept;

    private:
        std::unique_ptr<const CLIRegistry> registry_;
    };

}