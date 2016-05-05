<?hh // strict

newtype BindingCallback = (function(?array) : mixed);
newtype Binding = Pair<BindingCallback, bool>;

class IoC
{
    /*
     * name => (callback, isSingleton)
     */
    private static Map<string, Binding> $registry = Map{};

    /*
     * Map with the instantiated singleton objects
     */
    private static Map<string, mixed> $instances = Map{};

    /**
     * @param $name
     * @return string
     */
    private static function normalizeName(string $name) : string
    {
        return strtolower($name);
    }

    /**
     * @param Pair $registryData
     * @return bool
     */
    private static function isSingleton(Binding $registryData) : bool
    {
        return $registryData[1];
    }

    /**
     * @param Pair $registryData
     * @return function
     */
    private static function getCallback(Binding $registryData) : BindingCallback
    {
        return $registryData[0];
    }

    /**
     * Add a new resolver to the registry array.
     *
     *
     * @param string $name
     * @param function $resolver
     * @param bool $singleton
     */
    public static function bind(string $name, BindingCallback $resolver, bool $singleton = false) : void
    {
        $name = self::normalizeName($name);
        self::$registry[$name] = Pair{$resolver, $singleton};
    }

    /**
     * Add a new singleton resolver to the registry array.
     *
     *
     * @param string $name
     * @param function $resolver
     */
    public static function singleton(string $name, BindingCallback $resolver) : void
    {
        self::bind($name, $resolver, true);
    }

    /**
     * Create an instance
     *
     *
     * @param string $name
     * @param mixed $params
     * @param mixed $nameSuffix  Suffix for singleton instance keys
     *                           Example: (mysql-mysql://127.0.0.1/awesome_db) so we can have singleton instances for different MySql connections
     * @return object
     */
    public static function make<T>(classname<T> $name, ?array $params = null, string $instanceSuffix = '') : ?T
    {
        $name = self::normalizeName($name);
        if (static::has($name)) {
            $data = self::$registry[$name];
            if (self::isSingleton($data)) {
                $name .= $instanceSuffix;
                if (!self::hasInstance($name)) {
                    $callback = self::getCallback($data);
                    self::$instances[$name] = $callback($params);
                }

                return self::$instances[$name];
            }
            else {
                $callback = self::getCallback($data);
                return $callback($params);
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
    public static function has($name) : bool
    {
        return !empty(self::$registry[$name]);
    }

    /**
     * Determine whether the name is instantiated
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function hasInstance($name) : bool
    {
        return !empty(self::$instances[$name]);
    }

    /**
     * Remove an instance from the pool
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function flushInstance($name) : void
    {
        unset(self::$instances[$name]);
    }

    /**
     * Remove a Binding(callback) from the pool
     *
     *
     * @param string $name
     * @return boolean
     */
    public static function flushBinding($name) : void
    {
        unset(self::$registry[$name]);
    }
}
