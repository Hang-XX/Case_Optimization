#include "common.h"
#include "netlist.h"
#include "parser.h"
#include "optimization.h"
static Nl*      global_nl = nullptr;
static Parser*  global_parser = nullptr;

static void Initialize(const std::string& sourcefile, const std::string& csvfile) {
  if (nullptr == global_nl) {
    delete global_nl;
  }
  if (nullptr == global_parser) {
    delete global_parser;
  }

  global_nl = new Nl();
  global_parser = new Parser();

  global_parser->ReadVerilog(sourcefile);
  std::string model_decl = global_parser->GetModelDecl();
  global_nl->SetModelDecl(model_decl);

  global_parser->ReadLibCsv(csvfile);
  DEVICE_INFO device_info = global_parser->GetDeviceInfo();
  global_nl->SetDeviceInfo(device_info);
};
 
static void Clear() {
  delete global_nl;
  delete global_parser;
};

int main(int argc, char** argv) {
  // API demo for graph(6) in contest guide - begin

  // initial matrix
  // std::string filename("../case/simple_case/in.v");
  // std::string libcsv("../Libcell/lib1.csv");
  if (argc < 3) {
    std::cout << "Please specify an initialization file"
      << "\n The format is as follows:"
      << "\n ./td ../case/simple_case/in.v ../Libcell/lib1.csv"
      << std::endl;
    return -1;
  }
  std::string filename(argv[1]);
  std::string libcsv(argv[2]);
  std::cout << "*******************************" << std::endl;
  std::cout << "start initialize file : " << filename << ", " << libcsv << std::endl;
  Initialize(filename, libcsv);
  std::cout << "*******************************" << std::endl;

  // parser test
  std::cout << "*******************************" << std::endl;
  std::cout << "start print port range" << std::endl;
  RANGE din_range = global_parser->GetDinRange();
  std::cout << "din port range : [" << din_range.at(0) << ":" << din_range.at(1) << "]" << std::endl;
  RANGE dout_range = global_parser->GetDoutRange();
  std::cout << "dout port range : [" << dout_range.at(0) << ":" << dout_range.at(1) << "]" << std::endl;
  RANGE sel_range = global_parser->GetSelRange();
  std::cout << "sel port range : [" << sel_range.at(0) << ":" << sel_range.at(1) << "]" << std::endl;

  std::cout << "start print matrix" << std::endl;
  MATRIX addr_mat = global_parser->GetAddrMat();
  std::cout << "addMat : " << std::endl;
  for (auto& addr_vs : addr_mat) {
    for (auto& addr_v : addr_vs) {
      std::cout << " " << addr_v << " ";
    }
    std::cout << std::endl;
  }
  MATRIX data_mat = global_parser->GetDataMat();
  std::cout << "dataMat : " << std::endl;
  for (auto& data_vs : data_mat) {
    for (auto& data_v : data_vs) {
      std::cout << " " << data_v << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "*******************************" << std::endl;

  optimize_case(addr_mat, data_mat, din_range, dout_range, sel_range);
  



  // // Nl API test
  // // new inst mux/and/not
  // std::cout << "*******************************" << std::endl;
  // std::cout << "start create Nl" << std::endl;
  // Inst* muxInst1 = global_nl->CreateMux();
  // Inst* andInst1 = global_nl->CreateAnd();
  // Inst* notInst1 = global_nl->CreateNot();

  // // create module pin
  // Pin* din0 = global_nl->CreateModelInPin(0); // name : "din[0]", pin name format "din[n]", Note the index : n
  // Pin* din1 = global_nl->CreateModelInPin(1);
  // Pin* dout0 = global_nl->CreateModelOutPin(0); // name : "dout[0]", pin name format "dout[n]", Note the index : n
  // Pin* sel0 = global_nl->CreateModelSelPin(0); // name : "sel[0]", pin name format "sel[n]", Note the index : n

  // // connect pin 2 pin
  // global_nl->Connect(din0, notInst1->GetI0Pin());
  // global_nl->Connect(notInst1->GetOutPin(), andInst1->GetI0Pin());
  // global_nl->Connect(din1, andInst1->GetI1Pin());
  // global_nl->Connect(andInst1->GetOutPin(), muxInst1->GetI0Pin());
  // muxInst1->GetI1Pin()->Tie0(); // pin connect const 0, none net
  // global_nl->Connect(sel0, muxInst1->GetSelPin());
  // global_nl->Connect(muxInst1->GetOutPin(), dout0);
  // std::cout << "*******************************" << std::endl;

  // // Nl print test
  // // note : inst/inst_pin noname , net/module_pin have a name
  // std::cout << "*******************************" << std::endl;
  // std::cout 
  //   << "start print part Nl begin "
  //   << "\nmuxinst area:" << muxInst1->GetArea()
  //   << "\nmuxinst delay:" << muxInst1->GetDelay()
  //   << "\nmuxinst pin_net: " 
  //   << "\ni0 net : " << (muxInst1->GetI0Pin()->GetNet() ? muxInst1->GetI0Pin()->GetNet()->Name() : "const ")
  //   << "\ni1 net : " << (muxInst1->GetI1Pin()->GetNet() ? muxInst1->GetI1Pin()->GetNet()->Name() : "const ")
  //   << "\nsel net : " << (muxInst1->GetSelPin()->GetNet() ? muxInst1->GetSelPin()->GetNet()->Name() : "const ")
  //   << "\no net : " << (muxInst1->GetOutPin()->GetNet() ? muxInst1->GetOutPin()->GetNet()->Name() : "const ")
  //   << "\nmodule pin net: "
  //   << "\n pin name : " << din0->Name() << " connected net name : " << (din0->GetNet() ? din0->GetNet()->Name() : "const")
  //   << "\n pin name : " << sel0->Name() << " connected net name : " << (sel0->GetNet() ? sel0->GetNet()->Name() : "const")
  //   << "\n print test end " << std::endl;
  // std::cout << "*******************************" << std::endl;


  // // write verilog 
  // std::cout << "*******************************" << std::endl;
  // std::string out_file("../case/simple_case/out.v");
  // std::cout << "start cmd write verilog" << std::endl;
  // global_nl->WriteVerilog(out_file);
  // std::cout << "write verilog complete" << std::endl;
  // std::cout << "out file path: " << out_file << std::endl;
  // std::cout << "*******************************" << std::endl;

  // API demo for graph(6) in contest guide - end

  // clear global pointer
  Clear();
  return 0;
} // end of main

