#include "../package/broker/broker/broker.hpp"
#include <wfc/testing/testing_domain.hpp>
#include <fas/testing.hpp>
#include <memory>

namespace {

struct _test_;

wfc::iinterface::data_ptr make_data(const std::string& s)
{
  return std::make_unique<wfc::iinterface::data_type>( s.begin(), s.end() );
}

struct receiver: wfc::domain_object<wfc::iinterface, fas::empty_type>
{
  void virtual perform_io(data_ptr d, io_id_t, output_handler_t handler) override
  {
    handler(make_data(this->name() + ":" + std::string(d->begin(), d->end())));
  }
};

UNIT(init, "")
{
  using namespace fas::testing;
  GET_REF(_test_) = std::make_shared<wfc::testing_domain>();
  t << nothing;
}

UNIT(broker, "")
{
  using namespace fas::testing;
  using namespace wfc::io;

  auto tst = GET_REF(_test_);
  broker::domain_config bopt;
  bopt.name = "broker1";
  bopt.target = "receiver1";
  bopt.rules.resize(1);
  bopt.rules[0].target = "receiver2";
  bopt.rules[0].regex ="(sub)(.*)";
  auto br = tst->template create<broker>(bopt);
  receiver::domain_config ropt;
  ropt.name = "receiver1";
  auto rv1 = tst->template create<receiver>(ropt);
  ropt.name = "receiver2";
  auto rv2 = tst->template create<receiver>(ropt);
  tst->start();

  int count = 0;
  br->perform_io(make_data("object"), 1, [&t, &count](wfc::iinterface::data_ptr d){
    using namespace fas::testing;
    std::string ins(d->begin(), d->end() );
    t << equal_str<expect>(ins, "receiver1:object") << FAS_FL;
    ++count;
  });
  br->perform_io(make_data("subect"), 1, [&t, &count](wfc::iinterface::data_ptr d){
    using namespace fas::testing;
    std::string ins(d->begin(), d->end() );
    t << equal_str<expect>(ins, "receiver2:subect") << FAS_FL;
    ++count;
  });
  t << equal<expect>(count, 2) << FAS_FL;
  tst->stop();
}

}

BEGIN_SUITE(package_suite, "")
  ADD_VALUE( _test_, std::shared_ptr<wfc::testing_domain> )
  ADD_UNIT( init )
  ADD_UNIT( broker )
END_SUITE(package_suite)
