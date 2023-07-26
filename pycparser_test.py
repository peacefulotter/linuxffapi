from pycparser import c_ast, c_parser, c_generator, parse_file

# jclass (JNICALL* GetObjectClass)(struct JNIEnv* env, jobject obj);
text = r"""
struct environment {
    int a;
    int b;
} environment;

void func(char* arr, struct environment* env)
{
    int x;
    x = env->a;
    printf("%s", arr);
}
"""

def add_params(params):
    env = c_ast.Decl(name='env', quals=[], align=[], storage=[], )
    return [env, jobject, *params]

if __name__ == '__main__':

    # ast = parse_file('test_compiled.c') # , use_cpp=True, cpp_args=['-E', '-I/usr/lib/jvm/jdk-18/include/', '-I/usr/lib/jvm/jdk-18/include/linux'])
    # print(ast)
    
    parser = c_parser.CParser()
    ast = parser.parse(text)

    print("Before:")
    ast.show()

    func = ast.ext[1]
    params = func.decl.type.args.params
    print('Before', params)
    params = add_params(params)
    func.decl.type.args = c_ast.ParamList(params)
    print('After', func.decl.type.args)

    assign = func.body.block_items[0]
    assign.lvalue.name = "y"
    assign.rvalue.value = 2

    print("After:")
    ast.show(offset=2)

    # Write result to file
    # generator = c_generator.CGenerator()
    # print(generator.visit(ast))