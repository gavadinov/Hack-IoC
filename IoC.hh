<?hh // strict

namespace Container;

newtype IoCBindingCallback = (function(?array<mixed, mixed>) : mixed);
newtype IoCBinding = Pair<IoCBindingCallback, bool>;

class IoC
{
    /*
     * name => (callback, isSingleton)
     */
    private static Map<string, IoCBinding> $registry = Map{};

    /*
     * Map with the instantiated singleton objects
     */
    public static Map<string, mixed> $instances = Map{};

    /**
     * @param $name
     * @return string
     */
    private static function normalizeName<T>(classname<T> $name) : string
    {
        return strtolower((string)$name);
    }

    /**
     * @param Pair $registryData
     * @return bool
     */
    private static function isSingleton(IoCBinding $registryData) : bool
    {
        return $registryData[1];
    }

    /**
     * @param Pair $registryData
     * @return function
     */
    private static function getCallback(IoCBinding $registryData) : IoCBindingCallback
    {
        return $registryData[0];
    }

    /**
     * Add a new resolver to the registry array.
     *
     *
     * @param string $name
     * @param IoCBindingCallback $resolver
     * @param bool $singleton
     */
    public static function bind<T>(classname<T> $name, IoCBindingCallback $resolver, bool $singleton = false) : void
    {
        $name = self::normalizeName($name);
        self::$registry[$name] = Pair{$resolver, $singleton};
    }

    /**
     * Add a new singleton resolver to the registry array.
     *
     *
     * @param string $name
     * @param IoCBindingCallback $resolver
     */
    public static function singleton<T>(classname<T> $name, IoCBindingCallback $resolver) : void
    {
        self::bind($name, $resolver, true);
    }

    /**
     * Create an instance
     *
     *
     * @param classname $class
     * @param array $params
     * @param string $nameSuffix  Suffix for singleton instance keys
     *                           Example: (mysql-mysql://127.0.0.1/awesome_db) so we can have singleton instances for different MySql connections
     * @return object
     */
    public static function make<T>(classname<T> $class, ?array<mixed, mixed> $params = null, string $instanceSuffix = '') : ?T
    {
        $key = self::normalizeName($class);
        if (self::has($class)) {
            $data = self::$registry[$key];
            if (self::isSingleton($data)) {
                $key .= $instanceSuffix;
                if (!self::hasInstance($class, $instanceSuffix)) {
                    $callback = self::getCallback($data);
                    self::$instances[$key] = $callback($params);
                }

                $instance = self::$instances[$key];

                if ($instance instanceof $class) {
                    return $instance;
                }
            }
            else {
                $callback = self::getCallback($data);
                $instance = $callback($params);

                if ($instance instanceof $class) {
                    return $instance;
                }
            }
        }

        return null;
    }

    /**
     * Determine whether the name is registered
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function has<T>(classname<T> $name) : bool
    {
        $name = self::normalizeName($name);
        return !is_null(self::$registry->get($name));
    }

    /**
     * Determine whether the name is instantiated
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function hasInstance<T>(classname<T> $name, string $instanceSuffix = '') : bool
    {
        $name = self::normalizeName($name);
        $name .= $instanceSuffix;
        return !is_null(self::$instances->get($name));
    }

    /**
     * Remove an instance from the pool
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function flushInstance<T>(classname<T> $name, string $instanceSuffix = '') : void
    {
        $name = self::normalizeName($name);
        $name .= $instanceSuffix;

        self::$instances->remove($name);
    }

    /**
     * Remove a Binding(callback) from the pool
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function flushBinding<T>(classname<T> $name) : void
    {
        self::$registry->remove($name);
    }
}
