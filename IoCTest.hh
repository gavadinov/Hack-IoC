<?hh

require_once 'IoC.hh';

class DummyBindClass {}
class DummySingletonClass {}

class IoCTest extends PHPUnit_Framework_TestCase
{
    protected function setUp()
    {
        IoC::bind(DummyBindClass::class, function($params) {
            return new DummyBindClass();
        });

        IoC::singleton(DummySingletonClass::class, function($params) {
            return new DummySingletonClass();
        });
    }

    protected function tearDown()
    {
        IoC::flushBinding(DummyBindClass::class);
        IoC::flushBinding(DummySingletonClass::class);
        IoC::flushInstance(DummySingletonClass::class);
    }

    public function testBindReturnsNewInstances()
    {
        $instance1 = IoC::make(DummyBindClass::class);
        $instance2 = IoC::make(DummyBindClass::class);

        $this->assertNotSame($instance1, $instance2);
    }

    public function testSingletonReturnsSameInstances()
    {
        $instance1 = IoC::make(DummySingletonClass::class);
        $instance2 = IoC::make(DummySingletonClass::class);

        $this->assertSame($instance1, $instance2);
    }

    public function testReturnsInstanceOf()
    {
        $instance = IoC::make(DummyBindClass::class);

        $this->assertInstanceOf(DummyBindClass::class, $instance);
    }

    public function testBindingSuffix()
    {
        $instance1 = IoC::make(DummySingletonClass::class, [], 'suffix1');
        $instance2 = IoC::make(DummySingletonClass::class, [], 'suffix2');

        $this->assertNotSame($instance1, $instance2);
    }
}