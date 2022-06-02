#include "../package/tank/tank/tank_io.hpp"
#include <wfc/testing/testing_domain.hpp>
#include <wfc/iinterface.hpp>
#include <fas/testing.hpp>
#include <wjson/_json.hpp>
#include <memory>

namespace {


class tank_trap
  : public wfc::domain_object<wfc::iinterface, fas::empty_type>
{
public:
  std::vector<std::string> test;
  virtual void perform_io(data_ptr d, io_id_t /*io_id*/, output_handler_t handler) override
  {
    test.push_back( std::string(d->begin(), d->end()) );
    if ( handler!=nullptr )
      handler(nullptr);
  };
};

wfc::iinterface::data_type to_data(const std::string& str)
{
  return wfc::iinterface::data_type(str.begin(), str.end() );
}

UNIT(tank, "")
{
  using namespace fas::testing;
  using namespace wfc::io;
  using namespace wjson::literals;

{

  auto tst = std::make_shared<wfc::testing_domain>();

  tank_trap::domain_config trap_opt;
  trap_opt.name="trap1";
  auto trap = tst->template create<tank_trap>(trap_opt);

  tank_io::domain_config tank_opt;
  tank_opt.enable_vars = true;
  tank_opt.name = "tank1";
  tank_opt.target = "trap1";
  tank_opt.request = to_data("{'a':[$TANK{ID}, $TANK{COUNT2}, '$TANK{COUNT1[from=10, to=20, step=2]}=$TANK{COUNT1}']}"_json);
  tank_opt.jsonrpc_method = std::make_shared<std::string>("test");
  tank_opt.gunfire_max_waits = 10;
  tank_opt.gunfire_total=10;
  tank_opt.abort_if_bad_gateway = false;

  auto tmk = tst->template create<tank_io>(tank_opt);

  tst->start();
  for (int i=0; i < 10; ++i)
    tst->global()->io_context.run_one();
  tst->stop();

  t << equal<assert, size_t>(trap->test.size(), 10 ) << FAS_FL;
  t << flush;
  t << equal_str<expect>(trap->test[0], "{'jsonrpc':'2.0','method':'test','params':{'a':[1, 0, '10=10']},'id':1}"_json ) << FAS_FL;

  tmk = nullptr;
  trap = nullptr;
  tst->global()->clear();
}
  //tst->global()->clear();
  /*
  broker::domain_config bopt;
  bopt.name = 'broker1";
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
  */
}

}

BEGIN_SUITE(tank_suite, "")
  ADD_UNIT( tank )
END_SUITE(tank_suite)
