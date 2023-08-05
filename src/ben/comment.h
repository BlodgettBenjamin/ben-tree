#pragma once


// x----------------------------------------------------------------------------------------------x
// |   - use nice boxes like this because they are more epic                                      |
// |   - the intellisense will make the lines on the right get messed up but thats ok             |
// |   - three blank characters and then a hyphen and a bullet point                              |
// |   - leave a space under this comment box before any definitions or function headers          |
// |   - when you have something to say really really really really really really really really   |
// |     really really really really really really really really really really really really      |
// |     really really really really long like this remember to continue onto the next line but   |
// |     leave three black characters at the end and dont place another hyphen                    |
// x----------------------------------------------------------------------------------------------x
//
// 
// x----------------------------------------------------------------------------------------------x
// |   -                                                                                          |
// |   -                                                                                          |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// |                                                                                              |
// x----------------------------------------------------------------------------------------------x
// 
//          E     X      A     M     P     L     E      S      :    
// 
// x----------------------------------------------------------------------------------------------x
// |   -  btl::make_iterable is currently used for making btl::tree work with range based for     |
// |      loops                                                                                   |
// |   -  it could be implemented for types other than btl::tree in the future but                |
// |      disambiguation will make its use a bit more cumbersome                                  |
// x----------------------------------------------------------------------------------------------x
// |   EXAMPLE USAGE :                                                                            |
// x----------------------------------------------------------------------------------------------x
// |         for (const auto& vector : btl::make_iterable<vec3>(&tree)                            |
// |                                    or                                                        |
// |         for (const vec3& vector : btl::make_iterable<vec3>(&tree)                            |
// |                                                                                              |
// |                                   never                                                      |
// |         for (iterable<vec3, tree<>> vector : btl::make_iterable<vec3>(&tree)                 |
// x----------------------------------------------------------------------------------------------x
//
// 
// x----------------------------------------------------------------------------------------------x
// |   - catf concatenates a dynamic ben::stru64 buffer                                           |
// |   - it takes advantage of ben::str120s fixed size and vsnprintf to ensure buffer overflow    |
// |     does not occur                                                                           |
// |   - int the future it should call a btl::function for formatting strings that uses parame-   |
// |     ter packs and the assert for temp_buffer should be a part of ben::str120 functionality   |
// x----------------------------------------------------------------------------------------------x
//
// 
//