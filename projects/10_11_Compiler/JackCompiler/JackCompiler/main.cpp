#include <iostream>
#include <string>
#include <stdexcept> // For std::exception

#include "JackCompiler.h" // JackCompiler 클래스 헤더

int main(int argc, char* argv[]) {
    // 1. 커맨드 라인 인자가 정확히 하나인지 확인 (프로그램 이름 제외)
    if (argc != 2) {
        // cerr은 에러 메시지를 출력할 때 주로 사용
        std::cerr << "Usage: " << argv[0] << " <filename.jack or directory>" << std::endl;
        return 1; // 오류 종료 코드 반환
    }

    try {
        // 2. JackCompiler 객체 생성 (생성자에서 경로 처리)
        JackCompiler compiler(argv[1]);

        // 3. 컴파일 실행
        compiler.compile();

        std::cout << "Compilation successful." << std::endl;
        
    }
    catch (const std::exception& e) {
        // 컴파일 중 발생한 예외 처리 (파일 열기 실패, 문법 오류 등)
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1; // 오류 종료 코드 반환
    }

    return 0; // 정상 종료
}