import { Get, JsonController } from 'routing-controllers';
import { BaseController } from './BaseController';

@JsonController('/example')
export class ExampleController extends BaseController {
  constructor() {
    super();
  }

  // 예시
  @Get('/')
  public async getIndex() {
    return 'test!';
  }
}